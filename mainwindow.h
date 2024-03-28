#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "hanoitower.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
        Q_OBJECT

      private:
        HanoiTower *game = nullptr;
        QPixmap peg_a;
        QPixmap peg_b;
        QPixmap peg_c;
        void update_PegA();
        void update_PegB();
        void update_PegC();
        void updateAllPeg();
        void ParseCommand(QString);

      public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

      private slots:
        void on_lineEdit_returnPressed();

      private:
        Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
