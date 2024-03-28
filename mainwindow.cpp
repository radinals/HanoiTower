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
}

// parse input, convert to commands, and update peg sprites
void
MainWindow::ParseCommand(QString cmd)
{
	cmd = cmd.toLower();
	if (cmd == "pop(a)->b") {
		game->doAction(POP_A_TO_B);
		update_PegA();
		update_PegB();
	} else if (cmd == "pop(a)->c") {
		game->doAction(POP_A_TO_C);
		update_PegA();
		update_PegC();
	} else if (cmd == "pop(b)->a") {
		game->doAction(POP_B_TO_A);
		update_PegB();
		update_PegA();
	} else if (cmd == "pop(b)->c") {
		game->doAction(POP_B_TO_C);
		update_PegB();
		update_PegC();
	} else if (cmd == "pop(c)->a") {
		game->doAction(POP_C_TO_A);
		update_PegC();
		update_PegA();
	} else if (cmd == "pop(c)->b") {
		game->doAction(POP_C_TO_B);
		update_PegC();
		update_PegB();
	}

	if (game->gameIsWon()) {
		game->initPegs();
		updateAllPeg();
	}
}

void
MainWindow::on_lineEdit_returnPressed()
{
	QString input = ui->lineEdit->text();
	ParseCommand(input);
	ui->lineEdit->clear();
}
