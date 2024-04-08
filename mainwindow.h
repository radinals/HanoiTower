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
        void checkGameover();

      public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

      private slots:
        void on_Btn_PegAPopToB_clicked();

        void on_Btn_PegAPopToC_clicked();

        void on_Btn_PegBPopToA_clicked();

        void on_Btn_PegBPopToC_clicked();

        void on_Btn_PegCPopToA_clicked();

        void on_Btn_PegCPopToB_clicked();

      private:
        Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
