#pragma once

#include "customgamedialog.h"
#include "multibutton.h"

#include <QApplication>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QLocale>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QSettings>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QTranslator>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

class MainWindow : public QMainWindow
{
	Q_OBJECT

	bool DebugMode_;
	bool MinesShown_;

	QGridLayout *Grid_;

	QVector< QVector< MultiButton * > > MineField_;
	QVector< QVector< bool > > HasMine_;
	QVector< QVector< int > > MineCount_;

	QTranslator *Translator_;
	QLabel *CounterLabel_;

	int Rows_;
	int Columns_;
	int Mines_;
	int FlagsRaised_;

	int ClosedCells_;
	int OpenedCells_;

	bool FirstClick_;
	bool Inversion_;

	void createToolBar();
	void createInterface();
	void createField();
	void placeMines(const int IgnoredRow, const int IgnoredColumn);
	void openCell(int Row, int Column);
	void loseGame(QPushButton *Button);
	void toggleMines();
	void setLanguage(const QString &Language);
	void saveGameState(const QString &FileName);
	void loadGameState(const QString &FileName);

  private slots:
	void newGame();
	void customGame();
	void exitGame();
	void peek();
	void invertClick();
	void setEng();
	void setEs();
	void setRu();
	void handleLeftClick();
	void handleRightClick();
	void handleMiddleClick();

  protected:
	void resizeEvent(QResizeEvent *Event) override;
	void closeEvent(QCloseEvent *Event) override;

  public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
};
