/****************************************************************************
**
** diary.cpp
**
** Copyright (C) 2016 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of Diary.
**
** $BEGIN_LICENSE$
**
** This program is free software. It comes without any warranty, to
** the extent permitted by applicable law. You can redistribute it
** and/or modify it under the terms of the Do What The Fuck You Want
** To Public License, Version 2, as published by Sam Hocevar. See
** http://www.wtfpl.net/ for more details.
**
** $END_LICENSE$
**
****************************************************************************/

/** @file diary.cpp */

#include "diary.h"

#include <QtWidgets>
#include <QtSql>

/**
 * @brief '불러오기' 대화상자 클래스
 */
class LoadDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief LoadDialog 생성자
     * @param db 연결된 데이터베이스
     * @param parent 부모 위젯
     */
    LoadDialog(QSqlDatabase db, QWidget *parent = 0)
        : QDialog(parent)
    {
        // 모델 생성
        _model = new QSqlTableModel(this, db);
        _model->setTable("diary");
        // 직접 submitAll() 호출할 예정
        _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        // '날짜' 컬럼 이름 설정
        _model->setHeaderData(Diary_Date, Qt::Horizontal, tr("날짜"));
        // '제목' 컬럼 이름 설정
        _model->setHeaderData(Diary_Title, Qt::Horizontal, tr("제목"));
        // 데이터베이스에서 읽어들임
        _model->select();

        // 선택 모델 생성
        _selectionModel = new QItemSelectionModel(_model, this);

        // 테이블 뷰 생성
        _view = new QTableView;
        // 읽기 전용
        _view->setEditTriggers(QAbstractItemView::NoEditTriggers);
        // 탭 키 쓰지 않음
        _view->setTabKeyNavigation(false);
        // 하나만 선택
        _view->setSelectionMode(QAbstractItemView::SingleSelection);
        // 줄 단위로 선택
        _view->setSelectionBehavior(QAbstractItemView::SelectRows);

        // 모델 및 선택 모델 설정
        _view->setModel(_model);
        _view->setSelectionModel(_selectionModel);
        // 'ID' 와 '내용' 은 감춤
        _view->hideColumn(Diary_Id);
        _view->hideColumn(Diary_Content);
        // 내용에 맞게 컬럼 크기 조정
        _view->resizeColumnsToContents();
        // '날짜' 기준으로 내림차순 정렬
        _view->setSortingEnabled(true);
        _view->sortByColumn(Diary_Date, Qt::DescendingOrder);

        // 마지막 컬럼은 창 크기에 맞게 늘임
        _view->horizontalHeader()->setStretchLastSection(true);

        // 더블 클릭하면 해당 자료 불러들임
        connect(_view, SIGNAL(doubleClicked(QModelIndex)),
                this, SLOT(accept()));
        connect(_selectionModel,
                SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this, SLOT(showCurrentContent(QModelIndex)));

        // 읽기 전용으로 '내용' 편집기 생성
        _contentText = new QTextEdit;
        _contentText->setReadOnly(true);

        // '불러오기' 버튼 생성
        _loadButton = new QPushButton(tr("불러오기(&L)"));
        connect(_loadButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
        _loadButton->setDefault(true);

        // '지우기' 버튼 생성
        _deleteButton = new QPushButton(tr("지우기(&D)"));
        connect(_deleteButton, SIGNAL(clicked(bool)),
                this, SLOT(deleteDiary()));

        // '취소' 버튼 생성
        _cancelButton = new QPushButton(tr("취소(&C)"));
        connect(_cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

        QHBoxLayout *hboxLayout = new QHBoxLayout;
        hboxLayout->addStretch();
        hboxLayout->addWidget(_loadButton);
        hboxLayout->addStretch();
        hboxLayout->addWidget(_deleteButton);
        hboxLayout->addStretch();
        hboxLayout->addWidget(_cancelButton);
        hboxLayout->addStretch();

        QVBoxLayout *vboxLayout = new QVBoxLayout;
        vboxLayout->addWidget(_view);
        vboxLayout->addWidget(_contentText);
        vboxLayout->addLayout(hboxLayout);

        setLayout(vboxLayout);

        resize(500, 375);

        // 도움말 버튼 감추기
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        setWindowTitle(tr("불러오기"));
    }

    /**
     * @brief 일기 ID 를 돌려준다
     * @return 일기 ID
     */
    int id() const
    {
        return _record.value(Diary_Id).toInt();
    }

    /**
     * @brief 일기의 연도를 돌려준다
     * @return 일기의 연도
     */
    int year() const
    {
        return _record.value(Diary_Date).toDate().year();
    }

    /**
     * @brief 일기의 월을 돌려준다
     * @return 일기의 월
     */
    int month() const
    {
        return _record.value(Diary_Date).toDate().month();
    }

    /**
     * @brief 일기의 일을 돌려준다
     * @return 일기의 일
     */
    int day() const
    {
        return _record.value(Diary_Date).toDate().day();
    }

    /**
     * @brief 일기의 제목을 돌려준다
     * @return 일기의 제목
     */
    QString title() const
    {
        return _record.value(Diary_Title).toString();
    }

    /**
     * @brief 일기의 내용을 돌려준다
     * @return 일기의 내용
     */
    QString content() const
    {
        return _record.value(Diary_Content).toString();
    }

public slots:
    /**
     * @brief 사용자가 일기를 선택하면 대화상자를 마무리한다
     */
    void accept()
    {
        // 현재 선택된 일기
        QModelIndex index = _view->currentIndex();

        if (!index.isValid())
            return;

        // 일기 정보를 저장
        _record = _model->record(index.row());

        QDialog::accept();
    }

private:
    /** An enum of diary column
     */
    enum
    {
        Diary_Id = 0,       ///< 'ID' 컬럼
        Diary_Date = 1,     ///< '날짜' 컬럼
        Diary_Title = 2,    ///< '제목' 컬럼
        Diary_Content = 3   ///< '내용' 컬럼
    };

    QTableView *_view;          ///< 테이블 뷰
    QTextEdit *_contentText;    ///< '내용' 편집기
    QPushButton *_loadButton;   ///< '불러오기' 버튼
    QPushButton *_deleteButton; ///< '지우기' 버튼
    QPushButton *_cancelButton; ///< '취소' 버튼

    QSqlTableModel *_model;                 ///< SQL 모델
    QItemSelectionModel *_selectionModel;   ///< 선택 모델
    QSqlRecord _record;                     ///< 일기 정보

private slots:
    /**
     * @brief 선택된 일기를 지운다
     */
    void deleteDiary()
    {
        QModelIndex index = _view->currentIndex();

        if (!index.isValid())
            return;

        _model->removeRow(index.row());
        _model->submitAll();
    }

    /**
     * @brief 선택된 일기의 내용을 보여준다
     * @param current 선택된 모델 인덱스
     */
    void showCurrentContent(const QModelIndex &current)
    {
        _contentText->setText(_model->record(current.row())
                                .value("content").toString());
    }
};

/**
 * @brief Diary 생성자
 * @param parent 부모 위젯
 */
Diary::Diary(QWidget *parent)
    : QMainWindow(parent)
    , _yearCombo(0)
    , _monthCombo(0)
    , _dayCombo(0)
    , _id(-1)
    , _year(-1)
    , _month(-1)
    , _day(-1)
    , _db(QSqlDatabase::addDatabase("QSQLITE"))
{
    qApp->setApplicationName(tr("일기장"));

    // 데이터베이스 파일 설정
    _db.setDatabaseName(QApplication::applicationDirPath() + "/diary.sqlite");

    initMenus();
    initWidgets();

    newDiary();
}

/**
 * @brief Diary 소멸자
 */
Diary::~Diary()
{
}

/**
 * @brief 메뉴를 초기화한다
 */
void Diary::initMenus()
{
    // 파일 메뉴 생성
    QMenu *fileMenu = new QMenu(tr("파일(&F)"));
    fileMenu->addAction(tr("새 일기(&N)"), this, SLOT(newDiary()),
                        QKeySequence::New);
    fileMenu->addAction(tr("불러오기(&L)..."), this, SLOT(load()),
                        QKeySequence(tr("Ctrl+L")));
    fileMenu->addAction(tr("저장하기(&S)..."), this, SLOT(save()),
                        QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("끌내기(&x)"), this, SLOT(close()),
                        QKeySequence(tr("Ctrl+Q")));

    // 도움말 메뉴 생성
    QMenu *helpMenu = new QMenu(tr("도움말(&H)"));
    helpMenu->addAction(tr("%1 정보(&A)...").arg(qApp->applicationName()),
                        this, SLOT(about()));
    helpMenu->addAction(tr("Qt 정보(&Q)..."), this, SLOT(aboutQt()));

    // 메뉴바에 메뉴 추가
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(helpMenu);
}

/**
 * @brief 위젯을 초기화한다
 */
void Diary::initWidgets()
{
    // 연도 콤보 박스/레이블 생성 및 초기화
    _yearCombo = new QComboBox;
    for (int i = StartYear; i < EndYear; ++i)
        _yearCombo->addItem(QString::number(i));

    QLabel *yearLabel = new QLabel(tr("연도(&Y):"));
    yearLabel->setBuddy(_yearCombo);

    // 월 콤보 박스/레이블 생성 및 초기화
    _monthCombo = new QComboBox;
    for (int i = 1; i <= 12; ++i)
        _monthCombo->addItem(QString::number(i));

    QLabel *monthLabel = new QLabel(tr("월(&M):"));
    monthLabel->setBuddy(_monthCombo);

    // 일 콤보 박스/레이블 생성 및 초기화
    _dayCombo = new QComboBox;

    QLabel *dayLabel = new QLabel(tr("일(&D):"));
    dayLabel->setBuddy(_dayCombo);

    connect(_yearCombo, SIGNAL(currentIndexChanged(int)),this, SLOT(setDay()));
    connect(_monthCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setDay()));
    connect(_dayCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setDay()));

    QHBoxLayout *dateLayout = new QHBoxLayout;
    dateLayout->addStretch();
    dateLayout->addWidget(yearLabel);
    dateLayout->addWidget(_yearCombo);
    dateLayout->addStretch();
    dateLayout->addWidget(monthLabel);
    dateLayout->addWidget(_monthCombo);
    dateLayout->addStretch();
    dateLayout->addWidget(dayLabel);
    dateLayout->addWidget(_dayCombo);
    dateLayout->addStretch();

    // 제목 편집기/레이블 생성 및 초기화
    _titleLine = new QLineEdit;
    _titleLine->setMaxLength(80);
    connect(_titleLine, SIGNAL(textEdited(QString)),
            this, SLOT(setDiaryModified()));

    QLabel *titleLabel = new QLabel(tr("제목(&T):"));
    titleLabel->setBuddy(_titleLine);

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(_titleLine);

    // 내용 편집기 생성 및 초기화
    _contentText = new QTextEdit;
    connect(_contentText, SIGNAL(textChanged()),
            this, SLOT(setDiaryModified()));

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addLayout(dateLayout);
    vboxLayout->addLayout(titleLayout);
    vboxLayout->addWidget(_contentText);

    QWidget *w = new QWidget;
    w->setLayout(vboxLayout);

    setCentralWidget(w);

    resize(640, 480);
}

/**
 * @brief 창을 닫을 때 저장할지 확인한다
 * @param e 이벤트
 */
void Diary::closeEvent(QCloseEvent *e)
{
    if (askToSave())
        e->accept();
    else
        e->ignore();
}

/**
 * @brief 데이터베이스를 연다
 * @return 성공하면 true, 실패하면 false
 */
bool Diary::diaryOpenDb()
{
    if (!_db.isOpen() && !_db.open())
    {
        warning(tr("DB 를 열 수 없습니다."));

        return false;
    }

    return true;
}

/**
 * @brief 데이터베이스에 일기 테이블을 만든다
 * @return 성공하면 true, 실패하면 false
 */
bool Diary::diaryCreateTable()
{
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS diary ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "date DATE NOT NULL,"
                    "title VARCHAR(80) NOT NULL,"
                    "content TEXT NOT NULL"
                    ")"
                    ))
    {
        warning("테이블을 만들지 못했습니다.");

        return false;
    }

    return true;
}

/**
 * @brief 데이터베이스에서 일기를 찾는다
 * @param[out] query 질의 결과
 * @return 찾았으면 true, 못찾았으면 false
 */
bool Diary::diaryFind(QSqlQuery *query)
{
    query->prepare("SELECT id FROM diary WHERE id = :id");
    query->bindValue(":id", _id);
    return query->exec() && query->next();
}

/**
 * @brief 데이터베이스 일기를 저장한다
 * @param[out] query 질의 결과
 * @return 성공하면 true, 실패하면 false
 */
bool Diary::diaryInsert(QSqlQuery *query)
{
    query->prepare("INSERT INTO diary (id, date, title, content) "
                   "VALUES (:id, :date, :title, :content)");
    if (_id != -1)
        query->bindValue(":id", _id);
    else
        query->bindValue(":id", QVariant::Int);
    query->bindValue(":date", QDate(_year, _month, _day));
    query->bindValue(":title", _titleLine->text());
    query->bindValue(":content", _contentText->toPlainText());
    if (!query->exec())
    {
        warning(tr("일기를 저장하지 못했습니다."));

        return false;
    }

    if (_id == -1)
    {
        // 마지막에 삽입된 일기의 ID 를 얻는다
        if (!query->exec("SELECT last_insert_rowid() from diary") || !query->next())
        {
            warning(tr("DB 에서 일기 ID 를 얻지 못했습니다."));

            return false;
        }

        _id = query->value(0).toInt();
    }

    return true;
}

/**
 * @brief 데이터베이스에서 일기를 지운다
 * @param[out] query 질의 결과
 * @return 성공하면 true, 실패하면 false
 */
bool Diary::diaryDelete(QSqlQuery *query)
{
    query->prepare("DELETE FROM diary WHERE id = :id");
    query->bindValue(":id", _id);
    if (!query->exec())
    {
        warning(tr("일기를 지우지 못했습니다."));

        return false;
    }

    return true;
}

/**
 * @brief 데이터베이스 일기를 갱신한다
 * @param[out] query 질의 결과
 * @return 성공하면 true, 실패하면 false
 */
bool Diary::diaryUpdate(QSqlQuery *query)
{
    query->prepare("UPDATE diary "
                   "SET date = :date, title = :title, content = :content "
                   "WHERE id = :id");
    query->bindValue(":date", QDate(_year, _month, _day));
    query->bindValue(":title", _titleLine->text());
    query->bindValue(":content", _contentText->toPlainText());
    query->bindValue(":id", _id);
    if (!query->exec())
    {
        warning(tr("일기를 갱신하지 못했습니다."));

        return false;
    }

    return true;
}

/**
 * @brief 일기를 저장할지 물어본다
 * @return 계속 진행해도 되면 true, 중단해야 되면 false
 */
bool Diary::askToSave()
{
    if (isWindowModified())
    {
        QMessageBox::StandardButton button =
                QMessageBox::information(this, qApp->applicationName(),
                                         tr("바뀐 일기를 저장하시겠습니까?"),
                                         QMessageBox::Save |
                                         QMessageBox::Discard |
                                         QMessageBox::Cancel);

        return button == QMessageBox::Discard
                || (button == QMessageBox::Save && save());
    }

    return true;
}

/**
 * @brief 일기장에 대한 정보를 보여준다
 */
void Diary::about()
{
    // 컴파일한 날짜와 시간을 현지 시간으로 바꿈
    QString dateStr(__DATE__);
    if (dateStr.at(4) == ' ')
        dateStr[4] = '0';

    QDateTime dt;

    dt.setDate(QLocale(QLocale::C).toDate(dateStr, "MMM dd yyyy"));
    dt.setTime(QTime::fromString(__TIME__, "hh:mm:ss"));

    QMessageBox::about( this, tr("%1 정보").arg(qApp->applicationName()), tr(
"<h2>%1</h2>"
"<p>%2 에 만듦</p>"
"<p>이 프로그램에 대한 어떤 책임도 지지 않습니다. 이 프로그램은 공개 "
"소프트웨어이며, WTFPL v2 에 따라 재배포 및 수정될 수 있습니다.</p>"
"<p>자세한 것은 <a href=http://www.wtfpl.net>http://www.wtfpl.net</a> 를 "
"보십시오.</p>"
                      ).arg(qApp->applicationName())
                       .arg(dt.toString(Qt::SystemLocaleLongDate)));
}

/**
 * @brief 사용된 Qt 에 대한 정보를 보여준다
 */
void Diary::aboutQt()
{
    QMessageBox::aboutQt(this);
}

/**
 * @brief 일기의 일을 설정한다
 */
void Diary::setDay()
{
    QDate now(QDate::currentDate());

    // 콤보 박스를 연도/월/일로 바꿈
    int y = _yearCombo->currentIndex() + StartYear;
    int m = _monthCombo->currentIndex() + 1;
    int d = _dayCombo->currentIndex() + 1;

    // 해당 연도와 월의 일수를 얻음
    int daysInMonth = QDate(y, m, 1).daysInMonth();

    disconnect(_dayCombo, SIGNAL(currentIndexChanged(int)), this, 0);

    _dayCombo->clear();
    for (int i = 1; i <= daysInMonth; ++i)
        _dayCombo->addItem(QString::number(i));

    // 일기의 일을 조정
    if (d == 0)
        d = now.day();
    else if (d > daysInMonth)
        d = daysInMonth;

    _dayCombo->setCurrentIndex(d - 1);

    connect(_dayCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setDay()));

    // 조정된 연도/월/일 저장
    _year = y;
    _month = m;
    _day = d;

    setWindowTitle(title());

    // 사용자가 조정했을 때에만 바뀐 것으로 설정
    if (sender())
        setDiaryModified();
}

/**
 * @brief 문서 내용이 변경되었는지 여부를 설정한다
 * @param modified 변경되었으면 true, 아니면 false
 */
void Diary::setDiaryModified(bool modified)
{
    setWindowModified(modified);
}

/**
 * @brief 새 일기를 준비한다
 */
void Diary::newDiary()
{
    // 사용자가 호출했을 때만 저장 여부 확인
    if (sender() && !askToSave())
        return;

    // 멤버 변수 및 위젯 초기화
    _id = -1;

    QDate now(QDate::currentDate());

    _yearCombo->setCurrentIndex(now.year() - StartYear);
    _monthCombo->setCurrentIndex(now.month() - 1);
    _dayCombo->clear();

    setDay();

    _titleLine->clear();
    _contentText->clear();

    _contentText->setFocus();

    setDiaryModified(false);
}

/**
 * @brief 일기를 불러온다
 * @return 성공하면 true, 실패하면 false
 */
bool Diary::load()
{
    if (!askToSave())
        return false;

    if (!diaryOpenDb())
        return false;

    LoadDialog dlg(_db, this);

    if (dlg.exec() == QDialog::Accepted)
    {
        _id = dlg.id();
        _yearCombo->setCurrentIndex(dlg.year() - StartYear);
        _monthCombo->setCurrentIndex(dlg.month() - 1);
        _dayCombo->setCurrentIndex(dlg.day() - 1);
        _titleLine->setText(dlg.title());
        _contentText->setPlainText(dlg.content());

        setDiaryModified(false);
    }

    return true;
}

/**
 * @brief 일기를 저장한다
 * @return
 */
bool Diary::save()
{
    if (!isWindowModified())
    {
        if (QMessageBox::information(this, tr("저장하기"),
                                     tr("바뀐 내용이 없습니다. "
                                        "그래도 저장하시겠습니까?"),
                                     QMessageBox::Yes | QMessageBox::No)
                == QMessageBox::No)
            return true;
    }

    if (!diaryOpenDb() || !diaryCreateTable())
        return false;

    QSqlQuery query;
    bool ok = diaryFind(&query) ? diaryUpdate(&query) : diaryInsert(&query);

    if (ok)
        setDiaryModified(false);

    return ok;
}

// 소스 내부에서 선언된 클래스가 시그널/슬롯을 씀
#include "diary.moc"
