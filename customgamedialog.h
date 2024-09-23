#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

class QSpinBox;
class QDialogButtonBox;

class CustomGameDialog : public QDialog
{
	Q_OBJECT

  public:
	CustomGameDialog(QWidget *parent = nullptr);

	void updateRange();
	int getRows() const;
	int getColumns() const;
	int getMines() const;

  private:
	QSpinBox *RowsBox_;
	QSpinBox *ColumnBox_;
	QSpinBox *MinesBox_;
	QDialogButtonBox *ButtonBox_;
};
