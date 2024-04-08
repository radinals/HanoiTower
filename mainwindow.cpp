#include "mainwindow.h"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
        ui->setupUi(this);

        game = new HanoiTower();

        updateAllPeg();
}

void
MainWindow::updateAllPeg()
{
	update_PegA();
	update_PegC();
	update_PegB();
}

void
MainWindow::update_PegA()
{
	peg_a = game->getPegAPixmap();
	ui->PegA->setPixmap(peg_a);
}
void
MainWindow::update_PegB()
{
	peg_b = game->getPegBPixmap();
	ui->PegB->setPixmap(peg_b);
}

void
MainWindow::update_PegC()
{
	peg_c = game->getPegCPixmap();
	ui->PegC->setPixmap(peg_c);
}

MainWindow::~MainWindow()
{
	delete ui;
	delete game;
}

// check if a winstate is found, reset the game if found.
void
MainWindow::checkGameover()
{
	if (game->gameIsWon()) {
		game->initPegs();
		updateAllPeg();
	}
}

void
MainWindow::on_Btn_PegAPopToB_clicked()
{
	game->doAction(POP_A_TO_B);
	update_PegA();
	update_PegB();
	checkGameover();
}

void
MainWindow::on_Btn_PegAPopToC_clicked()
{
	game->doAction(POP_A_TO_C);
	update_PegA();
	update_PegC();
	checkGameover();
}

void
MainWindow::on_Btn_PegBPopToA_clicked()
{
	game->doAction(POP_B_TO_A);
	update_PegB();
	update_PegA();
	checkGameover();
}

void
MainWindow::on_Btn_PegBPopToC_clicked()
{
	game->doAction(POP_B_TO_C);
	update_PegB();
	update_PegC();
	checkGameover();
}

void
MainWindow::on_Btn_PegCPopToA_clicked()
{
	game->doAction(POP_C_TO_A);
	update_PegC();
	update_PegA();
	checkGameover();
}

void
MainWindow::on_Btn_PegCPopToB_clicked()
{
	game->doAction(POP_C_TO_B);
	update_PegC();
	update_PegB();
	checkGameover();
}
