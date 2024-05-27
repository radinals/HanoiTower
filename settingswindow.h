#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QGraphicsScene>
#include <QWidget>

namespace Ui
{
	class SettingsWindow;
}

class SettingsWindow : public QWidget
{
        Q_OBJECT

      private:
        struct Settings_t {
                size_t stack_amount = 0, slice_amount = 0;
                QColor slice_color, stack_color;
                long long int timer_ms = 0;
        } Settings;

        QGraphicsScene *m_preview_scene = nullptr;

	void drawPreview();
	void update_options();

      public:
        explicit SettingsWindow(QWidget *parent = nullptr);
        ~SettingsWindow();

      private slots:
        void on_GameStackAmountInput_editingFinished();

        void on_GameSliceAmountInput_editingFinished();

        void on_ThemeSliceColorSettingsInput_editingFinished();

        void on_ThemeStackColorSettingsInput_editingFinished();

        void on_GameTimerInput_editingFinished();

        void on_CancelButton_clicked();

        void on_SaveButton_clicked();

      private:
        Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
