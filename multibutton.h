#pragma once

#include <QMouseEvent>
#include <QPushButton>

class MultiButton : public QPushButton
{
	Q_OBJECT

	bool *Inversion_;

  public:
	MultiButton(QWidget *parent = nullptr, bool *Inversion = nullptr);

  signals:
	void leftClicked();
	void rightClicked();
	void middleClicked();

  protected:
	void mousePressEvent(QMouseEvent *Event) override;
};
