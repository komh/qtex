/****************************************************************************
**
** mainwindow.cpp
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of SyntaxHighlighter.
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

/** @file mainwindow.cpp
 */

#include "mainwindow.h"

/**
 * @brief 토큰 파서 클래스
 */
class TokenParser
{
public:
    /**
     * @brief TokenParser 생성자
     * @param s 파싱할 문자열
     */
    explicit TokenParser(const QString s = QString())
        : _s(s)
        , _currentPos(0)

    {
    }

    /**
     * @brief 남은 토큰이 있는지 확인
     * @return 토큰이 있으면 true, 없으면 false
     */
    bool hasNext() const
    {
        return _currentPos < _s.length();
    }

    /**
     * @brief 토큰을 읽고 다음으로 이동
     * @return 읽은 토큰
     */
    QString next()
    {
        return nextCommon();
    }

    /**
     * @brief 토큰을 읽지만 다음으로 이동하지 않음
     * @return 읽은 토큰
     */
    QString peekNext()
    {
        return nextCommon(false);
    }

    /**
     * @brief 현재 파싱 위치를 얻음
     * @return 현재 파싱 위치
     */
    int currentPos()
    {
        return _currentPos;
    }

    /**
     * @brief 파싱할 위치 설정
     * @param currentPos 새로운 파싱 위치
     */
    void setCurrentPos(int currentPos)
    {
        _currentPos = currentPos;
    }

private:
    QString _s;         /// 파싱할 문자열

    int _currentPos;    /// 파싱할 위치

    /**
     * @brief 토큰을 읽음
     * @param nextMode true 이면 다음으로 이동, 아니면 이동하지 않음
     * @return 읽은 토큰
     */
    QString nextCommon(bool nextMode = true)
    {
        int start = _currentPos;
        int end = _currentPos;
        QChar ch;

        // 연속된 문자, 숫자, _ 은 하나의 토큰
        while (end < _s.length()
               && ((ch = _s.at(end)).isLetterOrNumber() || ch == '_'))
            ++end;

        // 문자, 숫자, _ 가 아니면 한 문자가 하나의 토큰
        if (end < _s.length() && start == end
                && !((ch = _s.at(end)).isLetterOrNumber() || ch == '_'))
            ++end;

        // next mode 이면 파싱위치 이동
        if (nextMode)
            _currentPos = end;

        return _s.mid(start, end - start);
    }
};

/**
 * @brief 토큰 처리를 위한 추상 클래스
 */
class TokenAbstract
{
public:
    enum TokenType {Nothing = 0, Keyword, Block};

    /**
     * @brief 보통 텍스트를 HTML 텍스트로 바꿈
     * @param plain 보통 텍스트
     * @return HTML 텍스트
     */
    static QString plainToHtml(const QString &plain)
    {
        QString result;

        for (int i = 0; i < plain.length(); ++i)
        {
            QChar ch(plain.at(i));

            if (ch == ' ')
                result.append("&nbsp;");
            else if (ch == '\n')
                result.append("<br/>");
            else if (ch == '<')
                result.append("&lt;");
            else if (ch == '>')
                result.append("&gt;");
            else if (ch == '&')
                result.append("&amp;");
            else
                result.append(ch);
        }

        return result;
    }

    /**
     * @brief TokenAbstract 생성자
     * @param token 토큰
     * @param color 색
     */
    TokenAbstract(const QString &token, const QString &color)
        : _token(token)
        , _color(color)
    {
    }

    /**
     * @brief TokenAbstract 소멸자
     */
    virtual ~TokenAbstract() {}

    /**
     * @brief 토큰 타입을 얻음
     * @return 토큰 타입
     */
    virtual TokenType type() const = 0;

    /**
     * @brief 토큰이 일치하는지 확인
     * @param token 토큰. 일치하는 토큰으로 바뀜
     * @param parser 토큰 파서
     * @return 일치하면 true, 아니면 false
     */
    virtual bool matched(QString *token, TokenParser *parser) const = 0;

    /**
     * @brief 현재 토큰을 얻음
     * @return 현재 토큰
     */
    virtual QString token() const
    {
        return _token;
    }

    /**
     * @brief 토큰의 색을 얻음
     * @return 토큰의 색
     */
    virtual QString color() const
    {
        return _color;
    }

    /**
     * @brief HTML 텍스트를 얻음
     * @return HTML 텍스트
     */
    virtual QString html() const = 0;

private:
    QString _token; /// 토큰
    QString _color; /// 색
};

/**
 * @brief 키워드 토큰 클래스
 */
class TokenKeyword : public TokenAbstract
{
public:
    /**
     * @brief TokenKeyword 생성자
     * @param token 토큰
     * @param color 색
     */
    TokenKeyword(const QString &token, const QString &color)
        : TokenAbstract(token, color)
    {
    }

    bool matched(QString *token, TokenParser *parser) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(parser);

        return *token == this->token();
    }

    QString html() const Q_DECL_OVERRIDE
    {
        return QString("<span style=\"color:%1\">").arg(color())
                .append(plainToHtml(this->token()))
                .append("</span>");
    }

    TokenType type() const Q_DECL_OVERRIDE
    {
        return Keyword;
    }
};

/**
 * @brief 전처리기 지시자 클래스
 */
class TokenDirective : public TokenAbstract
{
public:
    /**
     * @brief TokenDirective 생성자
     * @param token 토큰
     * @param color 색
     * @param prefix 접두어
     */
    TokenDirective(const QString &token, const QString &color,
                    const QString &prefix = "#")
        : TokenAbstract(token, color)
        , _prefix(prefix)
        , _matched_token(prefix + token)
    {
    }

    bool matched(QString *token, TokenParser *parser) const Q_DECL_OVERRIDE
    {
        // 파싱 위치 저장
        int savedPos = parser->currentPos();

        QString prefix(*token);

        // 접두어 확인
        while (prefix.length() < _prefix.length() &&
               _prefix.startsWith(prefix) && parser->hasNext())
            prefix.append(parser->next());

        if (prefix == _prefix)
        {
            QString nextToken;

            // 공백문자나 탭문자는 넘어감
            while (parser->hasNext() &&
                   ((nextToken = parser->peekNext()) == " " ||
                    nextToken == "\t"))
                prefix.append(parser->next());

            QString tkword(TokenAbstract::token());
            QString word;

            // 단어 확인
            while (word.length() < tkword.length() &&
                   tkword.startsWith(word) && parser->hasNext())
                word.append(parser->next());

            if (word == tkword)
            {
                *token = _matched_token = prefix + word;

                return true;
            }
        }

        // 파싱 위치 복원
        parser->setCurrentPos(savedPos);

        return false;
    }

    QString token() const Q_DECL_OVERRIDE
    {
        return _matched_token;
    }

    QString html() const Q_DECL_OVERRIDE
    {
        return QString("<span style=\"color:%1\">").arg(color())
                .append(plainToHtml(this->token()))
                .append("</span>");
    }

    TokenType type() const Q_DECL_OVERRIDE
    {
        return Keyword;
    }

private:
    QString _prefix;                /// 접두어
    mutable QString _matched_token; /// 일치한 토큰
};

/**
 * @brief 블럭 토큰 클래스
 */
class TokenBlock : public TokenAbstract
{
public:
    /**
     * @brief TokenBlock 생성자
     * @param token 토큰
     * @param endToken 끝나는 토큰
     * @param color 색
     */
    TokenBlock(const QString &token, const QString &endToken,
               const QString &color)
        : TokenAbstract(token, color)
        , _startToken(token)
        , _endToken(endToken)
        , _started(false)
    {
    }

    bool matched(QString *token, TokenParser *parser) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(parser);

        QString tkblock(_started ? _endToken : _startToken);

        QString tk(*token);

        // 파싱 위치 저장
        int savedPos = parser->currentPos();

        // 토큰 확인
        while (tk.length() < tkblock.length() &&
               tkblock.startsWith(tk) && parser->hasNext())
            tk.append(parser->next());

        if (tk == tkblock)
        {
            // 토큰 시작 상태 바꿈
            _started = !_started;

            *token = tk;

            return true;
        }

        // 파싱 위치 복원
        parser->setCurrentPos(savedPos);

        return false;
    }

    QString token() const Q_DECL_OVERRIDE
    {
        return _started ? _startToken : _endToken;
    }

    QString html() const Q_DECL_OVERRIDE
    {
        QString tk(plainToHtml(this->token()));

        if (_started)
            tk.prepend(QString("<span style=\"color:%1;\">").arg(color()));
        else
            tk.append("</span>");

        return tk;
    }

    TokenType type() const Q_DECL_OVERRIDE
    {
        return Block;
    }

    /**
     * @brief 블럭 내부인지 확인
     * @return 블럭 내부이면 true, 아니면 false
     */
    bool inner() const
    {
        return _started;
    }

    /**
     * @brief 블럭 내부 상태 해제
     */
    void reset()
    {
        _started = false;
    }

private:
    QString _startToken;    /// 시작 토큰
    QString _endToken;      /// 끝 토큰
    mutable bool _started;  /// 시작 상태
};

/**
 * @brief MainWindow 생성자
 * @param parent 부모 위젯
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initMenus();    // 메뉴 초기화
    initWidgets();  // 위젯 초기화
}

/**
 * @brief MainWindow 소멸자
 */
MainWindow::~MainWindow()
{

}

/**
 * @brief 메뉴를 초기화함
 */
void MainWindow::initMenus()
{
    // "파일" 메뉴 생성
    QMenu *fileMenu = new QMenu(tr("파일(&F)"));
    // "끝내기" 액션 추가
    fileMenu->addAction(tr("끝내기(&x)"), this, SLOT(close()),
                        QKeySequence::Quit);

    // "파일" 메뉴 추가
    menuBar()->addMenu(fileMenu);
}

/**
 * @brief 위젯 초기화
 */
void MainWindow::initWidgets()
{
    // 원본 텍스트용 위젯 생성
    _plainText = new QTextEdit(this);
    // rich text 받지 않음
    _plainText->setAcceptRichText(false);
    // 기본 글꼴 설정
    _plainText->setStyleSheet("font-family:\"Courier New\";font-size:10pt;");
    // 현재 글꼴 설정
    _plainText->setFontFamily("Courier New");
    _plainText->setFontPointSize(10);

    // 현재 글꼴의 메트릭스 얻음
    QFontMetrics fm(_plainText->currentFont());
    // 위젯 최소 크기 설정. 80 문자 폭. 4 는 위젯의 내부 여백
    _plainText->setMinimumWidth(fm.averageCharWidth() * 81 + 4 +
                       _plainText->verticalScrollBar()->sizeHint().width());

    connect(_plainText, SIGNAL(textChanged()), this, SLOT(plainTextChanged()));

    // 문법 강조된 텍스트용 위젯 생성
    _syntaxText = new QTextEdit(this);
    // 읽기 전용
    _syntaxText->setReadOnly(true);

    // 문법 강조 버튼 생성
    _highlightButton = new QPushButton(tr("문법 강조(&H)"), this);
    // 문법 강조 버튼 작동 불가능하게
    _highlightButton->setEnabled(false);
    connect(_highlightButton, SIGNAL(clicked(bool)),
            this, SLOT(syntaxHighlight()));

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(_plainText);
    vboxLayout->addWidget(_syntaxText);
    vboxLayout->addWidget(_highlightButton);

    QWidget *w = new QWidget(this);
    w->setLayout(vboxLayout);

    setCentralWidget(w);

    // 내부 위젯 크기에 맞추어 크기 조절
    adjustSize();

    // 원본 텍스트 위젯의 최소 크기 0 으로 설정하여, 크기 축소 가능하게
    _plainText->setMinimumWidth(0);
}

/**
 * @brief 원본 텍스트가 바뀌었을 때 호출됨
 */
void MainWindow::plainTextChanged()
{
    // 스크롤 동기화 해체
    disconnect(_plainText->verticalScrollBar(), SIGNAL(valueChanged(int)),
               _syntaxText->verticalScrollBar(), SLOT(setValue(int)));

    disconnect(_syntaxText->verticalScrollBar(), SIGNAL(valueChanged(int)),
               _plainText->verticalScrollBar(), SLOT(setValue(int)));

    // 문법 강조 버튼 작동 가능하게
    _highlightButton->setEnabled(true);
}

/**
 * @brief 문법 강조하기
 */
void MainWindow::syntaxHighlight()
{
    TokenParser parser(_plainText->toPlainText());
    QString html;
    QList<TokenAbstract *> tokenTypes;

    // 블럭 토큰 추가
    tokenTypes.append(new TokenBlock("\"", "\"", "green"));
    tokenTypes.append(new TokenBlock("'", "'", "green"));
    tokenTypes.append(new TokenBlock("/*", "*/", "green"));
    tokenTypes.append(new TokenBlock("//", "\n", "green"));

    QStringList keywords;

    // 키워드 추가
    keywords << "asm" << "auto"
             << "bool" << "break"
             << "case" << "catch" << "cdecl" << "char" << "class" << "const"
                << "const_cast" << "continue"
             << "default" << "delete" << "double" << "do" << "dynamic_cast"
             << "else" << "enum" << "explicit" << "extern"
             << "far" << "float" << "for" << "friend"
             << "goto"
             << "huge"
             << "if" << "interrupt" << "int"
             << "long"
             << "mutable"
             << "namespace" << "near" << "new"
             << "operator"
             << "pascal" << "private" << "protected" << "public"
             << "register" << "reinterpret_cast" << "return"
             << "short" << "signed" << "sizeof" << "static" << "static_cast"
                << "struct" << "switch"
             << "template" << "this" << "throw" << "try" << "typedef"
                << "typename"
             << "union" << "unsigned" << "using"
             << "virtual" << "void" << "volatile"
             << "while"
             << "yield";

    // 특수 상수 추가
    keywords << "true" << "false"
             << "TRUE" << "FALSE"
             << "NULL";

    foreach (QString keyword, keywords)
        tokenTypes.append(new TokenKeyword(keyword, "#808000"));

    // 전처리기 지시자 추가
    QStringList directives;

    directives  << "define"
                << "elif" << "else" << "endif" << "error"
                << "if" << "ifdef" << "ifndef" << "include"
                << "line"
                << "pragma"
                << "undef"
                << "warning";

    foreach (QString directive, directives)
        tokenTypes.append(new TokenDirective(directive, "blue", "#"));

    // 기호 추가
    QStringList ops;

    ops << ">" << "<" << "{" << "}" << "(" << ")" << "[" << "]" << "+" << "-"
        << ":" << "&" << "!" << "|" << "=" << "~" << "?" << "." << ";"
        << "," << "%" << "^" << "/" << "*";

    foreach (QString op, ops)
        tokenTypes.append(new TokenKeyword(op, "red"));

    bool escaped = false;           // 탈출 문자 사용 여부
    TokenBlock *currentBlock = 0;   // 현재 블럭 토큰

    // 파싱
    while (parser.hasNext())
    {
        QString token = parser.next();

        if (!escaped)   // 탈출 문자가 사용되지 않았으면
        {
            TokenAbstract *tokenType;
            bool matched = false;

            // 토큰 확인
            foreach(tokenType, tokenTypes)
            {
                if (tokenType->matched(&token, &parser))
                {
                    matched = true;

                    break;
                }
            }

            if (matched) // 토큰 일치하면
            {
                if (!currentBlock)  // 블럭 내부가 아니면
                {
                    token = tokenType->html();

                    // 블럭 토큰이면 현재 블럭 토큰 설정
                    if (tokenType->type() == TokenAbstract::Block)
                        currentBlock = static_cast<TokenBlock *>(tokenType);
                }
                else    // 블럭 내부이면
                {
                    // 또다른 블럭이면 블럭 시작 상태 해제
                    if (tokenType->type() == TokenAbstract::Block &&
                            tokenType != currentBlock)
                        static_cast<TokenBlock *>(tokenType)->reset();

                    // 현재 블럭이 끝났으면
                    if (currentBlock == tokenType && !currentBlock->inner())
                    {
                        token = tokenType->html();

                        // 현재 블럭 토큰 없음
                        currentBlock = 0;
                    }
                    else
                        token = TokenAbstract::plainToHtml(token);
                }
            }
            else
                token = TokenAbstract::plainToHtml(token);
        }

        // 토큰 추가
        html.append(token);

        // 탈출 문자 ?
        escaped = !escaped && token == "\\";
    }

    // 추가된 토큰 해제
    qDeleteAll(tokenTypes);

    // HTML 전체 글꼴 설정
    html.prepend("<div style=\"font-family:Courier New;font-size:10pt;\">");
    html.append("</div>");

    // 문법 강조용 텍스트 설정
    _syntaxText->setHtml(html);

    // 문법 강조 위젯 스크롤바 설정
    _syntaxText->verticalScrollBar()->setValue(
                _plainText->verticalScrollBar()->value());

    // 스크롤 동기화
    connect(_plainText->verticalScrollBar(), SIGNAL(valueChanged(int)),
            _syntaxText->verticalScrollBar(), SLOT(setValue(int)));

    connect(_syntaxText->verticalScrollBar(), SIGNAL(valueChanged(int)),
            _plainText->verticalScrollBar(), SLOT(setValue(int)));

    // 문법 강조 버튼 작동 불가능하게
    _highlightButton->setEnabled(false);
}
