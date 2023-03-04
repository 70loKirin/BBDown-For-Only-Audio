#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <QProcess>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_browse_clicked();
    void on_cancelButton_clicker();
    void changeIfCutMusic();

private:
    Ui::Widget *ui;
    QString msg;
    bool if_cut_music = false;

};

#endif // WIDGET_H
