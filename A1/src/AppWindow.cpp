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
  auto it = menuActions.find(key);
  if (it != menuActions.end()) {
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
  QAction* quitAct = new QAction(tr("&Quit"), this);
  menuActions.insert({Qt::Key_Q, quitAct});

  // Alternatively, you could use: setShortcuts(Qt::CTRL + Qt::Key_P);
  quitAct->setShortcuts(QKeySequence::Quit);
  quitAct->setStatusTip(tr("Exits the file"));

  connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void AppWindow::createMenu() {
  m_menu_app = menuBar()->addMenu(tr("&Application"));

  for (auto& kv : menuActions) {
    m_menu_app->addAction(kv.second);
  }
}
