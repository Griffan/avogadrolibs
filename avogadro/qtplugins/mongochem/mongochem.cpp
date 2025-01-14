/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "mongochem.h"
#include "mongochemwidget.h"

#include <avogadro/io/fileformat.h>
#include <avogadro/io/fileformatmanager.h>
#include <avogadro/qtgui/molecule.h>

#include <QAction>
#include <QDebug>
#include <QDialog>
#include <QVBoxLayout>

namespace Avogadro::QtPlugins {

MongoChem::MongoChem(QObject* parent)
  : ExtensionPlugin(parent), m_action(new QAction(this))
{
  m_action->setText(tr("&MongoChem"));
  m_actions.push_back(m_action.data());
  connect(m_action.data(), &QAction::triggered, this,
          &MongoChem::menuActivated);
}

MongoChem::~MongoChem() = default;

QStringList MongoChem::menuPath(QAction*) const
{
  return { tr("&Extensions") };
}

void MongoChem::setMolecule(QtGui::Molecule* mol)
{
  m_molecule = mol;
}

bool MongoChem::readMolecule(QtGui::Molecule& mol)
{
  bool ok = Io::FileFormatManager::instance().readString(
    mol, m_moleculeData.data(), "cjson");

  if (ok)
    mol.setData("name", m_moleculeName.toStdString());

  m_moleculeData.clear();
  m_moleculeName.clear();

  return ok;
}

QString MongoChem::currentMoleculeCjson() const
{
  std::string ret;

  if (!m_molecule)
    return "";

  Io::FileFormatManager::instance().writeString(*m_molecule, ret, "cjson");
  return ret.c_str();
}

void MongoChem::menuActivated()
{
  if (!m_dialog) {
    m_dialog.reset(new QDialog(qobject_cast<QWidget*>(this)));
    auto* layout = new QVBoxLayout;
    layout->addWidget(new MongoChemWidget(this));
    m_dialog->setLayout(layout);
    m_dialog->setWindowTitle("MongoChem");
  }

  m_dialog->show();
}

void MongoChem::setMoleculeData(const QByteArray& data)
{
  m_moleculeData = data;
  emit moleculeReady(1);
}

} // namespace Avogadro
