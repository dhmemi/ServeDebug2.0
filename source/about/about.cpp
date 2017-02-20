#include "about.h"
#include "ui_about.h"
#include <QDesktopServices>
#include <QUrl>

about::about(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::about)
{
    ui->setupUi(this);
    connect(ui->textBrowser, &QTextBrowser::anchorClicked,this, &about::openUrl);
    setWindowTitle("关于");
}

about::~about()
{
    delete ui;
}

void about::openUrl(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}
