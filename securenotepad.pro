# -------------------------------------------------
# Project created by QtCreator 2010-02-25T18:07:41
# -------------------------------------------------
TARGET = securenotepad
TEMPLATE = app
SOURCES += main.cpp \
    MainWindow.cpp \
    ExeParser.cpp \
    NewPasswordDialog.cpp \
    AboutDialog.cpp \
    ChangePasswordDialog.cpp \
    PasswordInputDialog.cpp \
    Encryption.cpp \
    FlatButton.cpp \
    ConfirmNoPasswordDialog.cpp
HEADERS += MainWindow.h \
    ExeParser.h \
    NewPasswordDialog.h \
    AboutDialog.h \
    ChangePasswordDialog.h \
    PasswordInputDialog.h \
    Encryption.h \
    FlatButton.h \
    openssl/evp.h \
    openssl/md5.h \
    ConfirmNoPasswordDialog.h
FORMS += MainWindow.ui \
    NewPasswordDialog.ui \
    AboutDialog.ui \
    ChangePasswordDialog.ui \
    PasswordInputDialog.ui \
    ConfirmNoPasswordDialog.ui
RESOURCES += resources.qrc
LIBS += "openssl-0.9.8n-lib/lib/libcrypto.a"
RC_FILE = securenotepad.rc
