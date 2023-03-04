#include "widget.h"
#include "ui_widget.h"
#include <stdio.h>
#include <QTextCodec>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->lineEditBV->setText("BV1V84y1j7bn");
    ui->lineEditBeginTime->setText("1:00");
    ui->lineEditEndTime->setText("2:00");

    //连接信号与槽
    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(on_pushButton_browse_clicked()));

    connect(ui->pushButton_cancel,&QPushButton::clicked,this,&Widget::on_cancelButton_clicker);

    connect(ui->pushButton_commit,&QPushButton::clicked,[this]()
    {
        QMessageBox::information(this,"标题",msg);
    }
    );

    connect(ui->radioButtonIfCutMusic,SIGNAL(clicked(bool)),this,SLOT(changeIfCutMusic()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::changeIfCutMusic(){
    qDebug() << "选中与否： " << ui->radioButtonIfCutMusic->isChecked();
    if(ui->radioButtonIfCutMusic->isChecked()){
        if_cut_music = true;
    }
    else{
        if_cut_music = false;
    }
}

void Widget::on_pushButton_browse_clicked()
{
    QString bbdown_location = "D:\\software\\BBdown\\BBdown.exe";
    QString ffmpeg_location = "D:\\software\\ffmpeg\\bin\\ffmpeg.exe";
    QString save_location = "C:\\Users\\70loKirin\\Desktop";
    QString BV = ui->lineEditBV->text();
    QStringList options;
    //没有/c程序会无法执行 不知道为什么
    options << "/c" << bbdown_location << BV ;
    QProcess myprocess;
    qDebug() << "开始处理" << options;
    myprocess.start("cmd",options);
    //等待运行结束，以不阻塞的形式
    myprocess.waitForFinished();
    myprocess.waitForReadyRead();
    //命令行执行结果
    //转码之后才能正常显示
    auto msg_str = myprocess.readAllStandardOutput().toStdString();
    const char* ch = msg_str.c_str();
    msg=QString::fromLocal8Bit(ch);
    //qDebug()<<"执行脚本："<<msg;


    //匹配文件名
    int begin_name_pose = msg.indexOf("视频标题: ") + 6;
    int end_name_pose = msg.indexOf("\r\n",begin_name_pose);
    QString music_name = msg.mid(begin_name_pose, end_name_pose-begin_name_pose)+".m4a";
    qDebug()<<"歌名："<< music_name;
    //BBDown会把\变成. 很抽象 估计还有其他字符也会变 以后再说
    while(1){
        int number = music_name.indexOf("/");
        if(number!= -1){
            music_name.replace(number,1,".");
        }
        else {
            break;
        }
        qDebug()<<"文件名："<< music_name << music_name.size();
    }
    QString music_name_m4a = music_name;
    QString music_name_mp3 = music_name.replace(music_name.size()-3,3,"mp3");

    //切割
    if(if_cut_music){
        qDebug() << "是否切割：" << if_cut_music;

        //切割
        QStringList cutByFfmpeg;
        qDebug() << ffmpeg_location << "-i" << music_name_m4a << "-ss" << ui->lineEditBeginTime->text() << "-to" << ui->lineEditEndTime->text() << "save_"+music_name_mp3;
        cutByFfmpeg << "/c" << ffmpeg_location << "-i" << music_name_m4a << "-ss" << ui->lineEditBeginTime->text() << "-to" << ui->lineEditEndTime->text() << "save_"+music_name_mp3;
        myprocess.start("cmd",cutByFfmpeg);
        myprocess.waitForFinished();
        myprocess.waitForReadyRead();

        //提取封面
        QStringList pictureOutAndIn;
        qDebug() << ffmpeg_location << "-i" << music_name_m4a << "-map" << "0:v" << "-map" << "-0:V" << "-c" << "copy" << "cover.jpg" ;
        pictureOutAndIn << "/c" << ffmpeg_location << "-i" << music_name_m4a << "-map" << "0:v" << "-map" << "-0:V" << "-c" << "copy" << "cover.jpg" ;//<< "&&" << ffmpeg_location << "-i" << "save_"+music_name_mp3 << "-i" << "cover.jpg" << "-map" << "0:0" << "-map" << "1:0" << "-c" << "copy" << "-id3v2_version" << "3" << music_name_mp3 ;
        myprocess.start("cmd",pictureOutAndIn);
        myprocess.waitForFinished();
        myprocess.waitForReadyRead();

        //添加封面
        QStringList pictureOutAndIn1;
        qDebug() << ffmpeg_location << "-i" << "save_"+music_name_mp3 << "-i" << "cover.jpg" << "-map" << "0:0" << "-map" << "1:0" << "-c" << "copy" << "-id3v2_version" << "3" << music_name_mp3 ;
        pictureOutAndIn1 << "/c" << ffmpeg_location << "-i" << "save_"+music_name_mp3 << "-i" << "cover.jpg" << "-map" << "0:0" << "-map" << "1:0" << "-c" << "copy" << "-id3v2_version" << "3" << music_name_mp3 ;
        myprocess.start("cmd",pictureOutAndIn1);
        myprocess.waitForFinished();
        myprocess.waitForReadyRead();

        //删除原来的文件
        QStringList deletM4a;
        deletM4a << "/c" << "del" << music_name_m4a << "&& del" << "cover.jpg" << "&& del" << "save_"+music_name_mp3;
        myprocess.start("cmd",deletM4a);
        myprocess.waitForFinished();
        myprocess.waitForReadyRead();

    }

    //移动到指定位置
    QStringList move_music_to_designated_place;
    move_music_to_designated_place << "/c" << "move" << music_name_mp3 << save_location;
    qDebug() << "开始移动：" << move_music_to_designated_place ;
    myprocess.start("cmd",move_music_to_designated_place);
    myprocess.waitForFinished();
    myprocess.waitForReadyRead();
    auto msg_str2 = myprocess.readAllStandardOutput().toStdString();
    const char* ch2 = msg_str2.c_str();
    msg = QString::fromLocal8Bit(ch2);
    qDebug()<<"移动结果："<<msg;
    myprocess.close();


}

void Widget::on_cancelButton_clicker()
{
    this->close();
}
