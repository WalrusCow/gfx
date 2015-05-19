#include <QtWidgets>
#include <QGLFormat>
#include <iostream>
#include "AppWindow.hpp"

AppWindow::AppWindow() : game(10, 20) {
  setWindowTitle("488 Tetrominoes on the Wall");

  QGLFormat glFormat;
  glFormat.setVersion(3, 3);
  glFormat.setProfile(QGLFormat::CoreProfile);
  glFormat.setSampleBuffers(true);

  QVBoxLayout *layout = new QVBoxLayout;
  // m_menubar = new QMenuBar;
  m_viewer = new Viewer(glFormat, this);
  layout->addWidget(m_viewer);
  setCentralWidget(new QWidget);
  centralWidget()->setLayout(layout);

  m_viewer->viewGame(&game);
  m_viewer->show();

  createActions();
  createMenu();
}

void AppWindow::keyPressEvent(QKeyEvent *event) {
  int key = event->key();
  auto it = shortcutActions.find(key);
  if (it != shortcutActions.end()) {
    // We have a matching menu shortcut
    it->second->trigger();
    return;
  }

  switch (key) {
    case Qt::Key_Escape:
      QCoreApplication::instance()->quit();
      break;
    case Qt::Key_T:
      std::cerr << "Hello!" << std::endl;
      break;
    default:
      QWidget::keyPressEvent(event);
  }
}

void AppWindow::newGame() {
  game.reset();
  m_viewer->viewGame(&game);
}

void AppWindow::createActions() {
  newMenuAction("&Quit", "Exit the program", appMenuActions, Qt::Key_Q, [this] {
    QMainWindow::close();
  })->setShortcuts(QKeySequence::Quit);

  newMenuAction("&Reset", "Reset the view", appMenuActions, Qt::Key_R, [this] {
    m_viewer->resetView();
  });

  newMenuAction("&New Game", "Start a new game", appMenuActions, Qt::Key_N, [this] {
    newGame();
  });

  newMenuAction(
      "&Wire-frame", "Draw wire frames", drawMenuActions, Qt::Key_W, [this] {
    m_viewer->setMode(Viewer::DrawMode::WIRE);
  });

  newMenuAction("&Face", "Fill faces", drawMenuActions, Qt::Key_F, [this] {
    m_viewer->setMode(Viewer::DrawMode::FACE);
  });
}

void AppWindow::createMenu() {
  appMenu = menuBar()->addMenu(tr("&Application"));
  for (auto& kv : appMenuActions) {
    appMenu->addAction(kv);
  }

  drawMenu = menuBar()->addMenu(tr("&Draw Mode"));
  for (auto& kv : drawMenuActions) {
    drawMenu->addAction(kv);
  }
}

QAction* AppWindow::newMenuAction(
    const std::string& title,
    const std::string& tip,
    std::list<QAction*>& menuList,
    int shortcut,
    const std::function<void()>& onTrigger) {

  QAction* action = new QAction(tr(title.c_str()), this);
  action->setStatusTip(tr(tip.c_str()));
  shortcutActions.insert({shortcut, action});
  menuList.push_back(action);
  connect(action, &QAction::triggered, this, onTrigger);
  return action;
}
