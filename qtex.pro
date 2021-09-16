TEMPLATE = subdirs

SUBDIRS += \
    Hello \
    BMI \
    Baseball \
    Puzzle \
    Calc \
    TimeTable \
    SyntaxHighlighter \
    DirLister \
    Clock \
    Plot \
    Tetris \
    Diary \
    mpgui \
    lvplayer

qtHaveModule(webkitwidgets): SUBDIRS += Exchange
