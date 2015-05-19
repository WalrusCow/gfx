#include <QtWidgets>
#include <QGLFormat>
#include <iostream>
#include "AppWindow.hpp"

AppWindow::AppWindow() {
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

void AppWindow::createActions() {
  QAction* quitAct = newAction("&Quit", "Exits the program", Qt::Key_Q);
  appMenuActions.push_back(quitAct);

  // Alternatively, you could use: setShortcuts(Qt::CTRL + Qt::Key_P);
  quitAct->setShortcuts(QKeySequence::Quit);
  connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

  // TODO: status tips

  QAction* resetAct = newAction("&Reset", "Reset the view", Qt::Key_R);
  appMenuActions.push_back(resetAct);
  connect(resetAct, &QAction::triggered, this, [this] {
    m_viewer->resetView();
  });

  QAction* wireModeAct = newAction(
      "&Wire-frame", "Draw wire frames", Qt::Key_W);
  drawMenuActions.push_back(wireModeAct);
  connect(wireModeAct, &QAction::triggered, this, [this] {
    m_viewer->setMode(Viewer::DrawMode::WIRE);
  });

  QAction* faceModeAct = newAction("&Face", "Fill faces", Qt::Key_F);
  drawMenuActions.push_back(faceModeAct);
  connect(faceModeAct, &QAction::triggered, this, [this] {
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

QAction* AppWindow::newAction(
    const std::string& title, const std::string& tip, int key) {
  QAction* action = new QAction(tr(title.c_str()), this);
  action->setStatusTip(tr(tip.c_str()));
  shortcutActions.insert({key, action});
  return action;
}
