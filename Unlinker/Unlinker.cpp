#include "Unlinker.h"
#include <qmessagebox.h>

Unlinker::Unlinker(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UnlinkerClass())
{
    ui->setupUi(this);
    this->m_client = new UnlinkerClient(this);

    if (!this->m_client->isDriverAvailable()) {
        QMessageBox::warning(this, "Driver not found", "Please install Driver before execute this program", QMessageBox::Ok);
        exit(EXIT_FAILURE);
    }

    connect(ui->actionExit, &QAction::triggered, [=]() {
        exit(EXIT_FAILURE);
    });

    connect(ui->actionAbout_Unlinker, &QAction::triggered, [=]() {
        QMessageBox::information(this, "About Unlinker", "[Unlinker] Hidding userland process.\n\n Developer: un4ckn0wl3z", QMessageBox::Ok);
    });

    connect(ui->hideButton, &QPushButton::clicked, [=]() {
        if (ui->pidLineEdit->text() == "") return;
        bool result = this->m_client->hide(ui->pidLineEdit->text().toInt());
        if (!result) {
            QMessageBox::critical(this, "Error", "Cannot hide target process", QMessageBox::Ok);
            ui->statusBar->showMessage("Hidden processId: " + ui->pidLineEdit->text() + " failed");
            return;
        }
        QMessageBox::information(this, "Success", "Target process hidden!", QMessageBox::Ok);
        ui->statusBar->showMessage("Hidden processId: " + ui->pidLineEdit->text() + " successfully");
    });
}

Unlinker::~Unlinker()
{
    delete ui;
}
