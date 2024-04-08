#include "mainwindow.h"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
        ui->setupUi(this);

	game = new HanoiTower();
	msgbox = new QMessageBox();

	// TODO: tidy up formating and styling.

	font = new QFont("Sans Serif", 18);

	msgbox->setFont(*this->font);
	msgbox->setStyleSheet("QLabel{min-width: 700px;}");

	msgbox->setStandardButtons(QMessageBox::Ok);

	updateAllPeg();

	showPopup("Pindahkan semua balok dari A ke C dengan urutan yang sama!");
}

bool
MainWindow::sendGameAction(GameActions action)
{
	// TODO: Re-Do Exceptions!
	try {
		game->doAction(action);
		return true;
	} catch (std::runtime_error err) {
		if (strcmp(err.what(),
			   "HanoiTower: Cannot Pop to lesser Slice")) {
			showPopup(
			    "Tidak bisa memindahkan balok yang lebih besar keatas balok yang lebih kecil!");
		}
	} catch (std::out_of_range err) {
		showPopup("Masih kosong!, Tidak ada yang bisa dipindahkan!");
	}
	return false;
}

void
MainWindow::showPopup(const QString& message)
{
	msgbox->setText(message);
	msgbox->show();
	msgbox->raise();
	msgbox->activateWindow();
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
	delete msgbox;
	delete font;
}

// check if a winstate is found, reset the game if found.
void
MainWindow::checkGameover()
{
	if (game->gameIsWon()) {
		showPopup("Gameover: Selamat, anda menang!");
		game->initPegs();
		updateAllPeg();
	}
}

void
MainWindow::on_Btn_PegAPopToB_clicked()
{
	if (sendGameAction(POP_A_TO_B)) {
		update_PegA();
		update_PegB();
		checkGameover();
	}
}

void
MainWindow::on_Btn_PegAPopToC_clicked()
{
	if (sendGameAction(POP_A_TO_C)) {
		update_PegA();
		update_PegC();
		checkGameover();
	}
}

void
MainWindow::on_Btn_PegBPopToA_clicked()
{
	if (sendGameAction(POP_B_TO_A)) {
		update_PegB();
		update_PegA();
		checkGameover();
	}
}

void
MainWindow::on_Btn_PegBPopToC_clicked()
{
	if (sendGameAction(POP_B_TO_C)) {
		update_PegB();
		update_PegC();
		checkGameover();
	}
}

void
MainWindow::on_Btn_PegCPopToA_clicked()
{
	if (sendGameAction(POP_C_TO_A)) {
		update_PegC();
		update_PegA();
		checkGameover();
	}
}

void
MainWindow::on_Btn_PegCPopToB_clicked()
{
	if (sendGameAction(POP_C_TO_B)) {
		update_PegC();
		update_PegB();
		checkGameover();
	}
}
