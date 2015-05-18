#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <unordered_map>
#include <vector>

#include "Viewer.hpp"

class AppWindow : public QMainWindow {
  Q_OBJECT

 public:
  AppWindow();

 protected:
  void keyPressEvent(QKeyEvent *event);

 private:
  void createActions();
  void createMenu();

  // Each menu itself
  QMenu* m_menu_app;

  std::unordered_map<int, QAction*> menuActions;
  Viewer* m_viewer;
};
