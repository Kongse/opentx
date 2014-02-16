#include "apppreferencesdialog.h"
#include "ui_apppreferencesdialog.h"
#include "mainwindow.h"
#include "helpers.h"
#include "flashinterface.h"
#ifdef JOYSTICKS
#include "joystick.h"
#include "joystickdialog.h"
#endif
#include <QDesktopServices>
#include <QtGui>

appPreferencesDialog::appPreferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::appPreferencesDialog)
{
  ui->setupUi(this);
  setWindowIcon(CompanionIcon("apppreferences.png"));
  initSettings();
  connect(this, SIGNAL(accepted()), this, SLOT(writeValues()));
#ifndef JOYSTICKS
  ui->joystickCB->hide();
  ui->joystickCB->setDisabled(true);
  ui->joystickcalButton->hide();
  ui->joystickChkB->hide();
  ui->label_11->hide();
#endif
  resize(0,0);
}

appPreferencesDialog::~appPreferencesDialog()
{
  delete ui;
}

void appPreferencesDialog::writeValues()
{
  glob.startup_check_companion(ui->startupCheck_companion9x->isChecked());
  glob.startup_check_fw(ui->startupCheck_fw->isChecked());
  glob.wizardEnable(ui->wizardEnable_ChkB->isChecked());
  glob.show_splash(ui->showSplash->isChecked());
  glob.simuSW(ui->simuSW->isChecked());
  glob.history_size(ui->historySize->value());
  glob.backLight(ui->backLightColor->currentIndex());
  glob.libraryPath(ui->libraryPath->text());
  glob.gePath(ui->ge_lineedit->text());
  glob.embedded_splashes(ui->splashincludeCB->currentIndex());
  glob.backupEnable(ui->backupEnable->isChecked());

  if (ui->joystickChkB ->isChecked() && ui->joystickCB->isEnabled()) {
    glob.js_support(ui->joystickChkB ->isChecked());  
    glob.js_ctrl(ui->joystickCB ->currentIndex());
  }
  else {
    glob.js_support(false);
    glob.js_ctrl(0);
  }

  glob.default_channel_order(ui->channelorderCB->currentIndex());
  glob.default_mode(ui->stickmodeCB->currentIndex());
  glob.rename_firmware_files(ui->renameFirmware->isChecked());
  glob.burnFirmware(ui->burnFirmware->isChecked());
  glob.profileId(ui->profileIndexLE->text().toInt());
  glob.Name(ui->profileNameLE->text());
  glob.sdPath(ui->sdPath->text());
  glob.SplashFileName(ui->SplashFileName->text());
  if (!ui->SplashFileName->text().isEmpty())
    glob.SplashImage("");
  glob.firmware(ui->firmwareLE->text());
  
  saveProfile();
}

void appPreferencesDialog::on_snapshotPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your snapshot folder"), glob.snapshotpath());
  if (!fileName.isEmpty()) {
    glob.snapshotpath(fileName);
    glob.snapshot_to_clipboard(false);
    ui->snapshotPath->setText(fileName);
  }
}

void appPreferencesDialog::initSettings()
{
  ui->snapshotClipboardCKB->setChecked(glob.snapshot_to_clipboard());
  ui->burnFirmware->setChecked(glob.burnFirmware());
  
  QString Path=glob.snapshotpath();
  if (QDir(Path).exists()) {
    ui->snapshotPath->setText(Path);
    ui->snapshotPath->setReadOnly(true);
  }
  if (ui->snapshotClipboardCKB->isChecked()) {
    ui->snapshotPath->setDisabled(true);
    ui->snapshotPathButton->setDisabled(true);
  }
  ui->startupCheck_companion9x->setChecked(glob.startup_check_companion());
  ui->startupCheck_fw->setChecked(glob.startup_check_fw());
  ui->wizardEnable_ChkB->setChecked(glob.wizardEnable());
  ui->showSplash->setChecked(glob.show_splash());
  ui->historySize->setValue(glob.history_size());
  ui->backLightColor->setCurrentIndex(glob.backLight());
  ui->simuSW->setChecked(glob.simuSW());

  Path=glob.libraryPath();
  if (QDir(Path).exists()) {
    ui->libraryPath->setText(Path);
  }
  Path=glob.gePath();
  if (QFile(Path).exists()) {
    ui->ge_lineedit->setText(Path);
  }  
  Path=glob.backupPath();
  if (!Path.isEmpty()) {
    if (QDir(Path).exists()) {
      ui->backupPath->setText(Path);
      ui->backupEnable->setEnabled(true);
      ui->backupEnable->setChecked(glob.backupEnable());
    } else {
      ui->backupEnable->setDisabled(true);
    }
  } else {
      ui->backupEnable->setDisabled(true);
  }
  ui->splashincludeCB->setCurrentIndex(glob.embedded_splashes());

#ifdef JOYSTICKS
  ui->joystickChkB->setChecked(glob.js_support());
  if (ui->joystickChkB->isChecked()) {
    QStringList joystickNames;
    joystickNames << tr("No joysticks found");
    joystick = new Joystick(0,false,0,0);
    ui->joystickcalButton->setDisabled(true);
    ui->joystickCB->setDisabled(true);

    if ( joystick ) {
      if ( joystick->joystickNames.count() > 0 ) {
        joystickNames = joystick->joystickNames;
        ui->joystickCB->setEnabled(true);
        ui->joystickcalButton->setEnabled(true);
      }
      joystick->close();
    }
    ui->joystickCB->clear();
    ui->joystickCB->insertItems(0, joystickNames);
    ui->joystickCB->setCurrentIndex(glob.js_ctrl());
  }
  else {
    ui->joystickCB->clear();
    ui->joystickCB->setDisabled(true);
    ui->joystickcalButton->setDisabled(true);
  }
#endif  
//  Profile Tab Inits  
  ui->channelorderCB->setCurrentIndex(glob.default_channel_order());
  ui->stickmodeCB->setCurrentIndex(glob.default_mode());
  ui->renameFirmware->setChecked(glob.rename_firmware_files());
  Path=glob.sdPath();
  if (QDir(Path).exists()) {
    ui->sdPath->setText(Path);
  }
  ui->profileIndexLE->setText(QString(glob.profileId()));
  ui->profileNameLE->setText(glob.Name());

  QString fileName=glob.SplashFileName();
  if (!fileName.isEmpty()) {
    QFile file(fileName);
    if (file.exists()){ 
      ui->SplashFileName->setText(fileName);
      displayImage( fileName );
    }
  }
  ui->firmwareLE->setText(glob.firmware());
}

void appPreferencesDialog::on_libraryPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your library folder"), glob.libraryPath());
  if (!fileName.isEmpty()) {
    glob.libraryPath(fileName);
    ui->libraryPath->setText(fileName);
  }
}

void appPreferencesDialog::on_snapshotClipboardCKB_clicked()
{
  if (ui->snapshotClipboardCKB->isChecked()) {
    ui->snapshotPath->setDisabled(true);
    ui->snapshotPathButton->setDisabled(true);
    glob.snapshot_to_clipboard(true);
  } else {
    ui->snapshotPath->setEnabled(true);
    ui->snapshotPath->setReadOnly(true);
    ui->snapshotPathButton->setEnabled(true);
    glob.snapshot_to_clipboard(false);
  }
}

void appPreferencesDialog::on_backupPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your Models and Settings backup folder"), glob.backupPath());
  if (!fileName.isEmpty()) {
    glob.backupPath(fileName);
    ui->backupPath->setText(fileName);
  }
  ui->backupEnable->setEnabled(true);
}

void appPreferencesDialog::on_ge_pathButton_clicked()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Select Google Earth executable"),ui->ge_lineedit->text());
  if (!fileName.isEmpty()) {
    ui->ge_lineedit->setText(fileName);
  }
}
 
#ifdef JOYSTICKS
void appPreferencesDialog::on_joystickChkB_clicked() {
  if (ui->joystickChkB->isChecked()) {
    QStringList joystickNames;
    joystickNames << tr("No joysticks found");
    joystick = new Joystick(0,false,0,0);
    ui->joystickcalButton->setDisabled(true);
    ui->joystickCB->setDisabled(true);

    if ( joystick ) {
      if ( joystick->joystickNames.count() > 0 ) {
        joystickNames = joystick->joystickNames;
        ui->joystickCB->setEnabled(true);
        ui->joystickcalButton->setEnabled(true);
      }
      joystick->close();
    }
    ui->joystickCB->clear();
    ui->joystickCB->insertItems(0, joystickNames);
  }
  else {
    ui->joystickCB->clear();
    ui->joystickCB->setDisabled(true);
    ui->joystickcalButton->setDisabled(true);
  }
}

void appPreferencesDialog::on_joystickcalButton_clicked() {
   joystickDialog * jd=new joystickDialog(this, ui->joystickCB->currentIndex());
   jd->exec();
}
#endif

// ******** Profile tab functions

void appPreferencesDialog::on_sdPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select the folder replicating your SD structure"), glob.sdPath());
  if (!fileName.isEmpty()) {
    ui->sdPath->setText(fileName);
  }
}

void appPreferencesDialog::saveProfile()
{
  QSettings settings;

  QString profile=QString("profile") + glob.profileId();
  QString name=ui->profileNameLE->text();
  if (name.isEmpty()) {
    name = profile;
    ui->profileNameLE->setText(name);
  }
  settings.beginGroup("Profiles");
  settings.beginGroup(profile);
  settings.setValue("Name",name);
  settings.setValue("default_channel_order", ui->channelorderCB->currentIndex());
  settings.setValue("default_mode", ui->stickmodeCB->currentIndex());
  settings.setValue("burnFirmware", ui->burnFirmware->isChecked());
  settings.setValue("rename_firmware_files", ui->renameFirmware->isChecked());
  settings.setValue("sdPath", ui->sdPath->text());
  settings.setValue("SplashFileName", ui->SplashFileName->text());
  settings.setValue("firmware", ui->firmwareLE->text());
  settings.endGroup();
  settings.endGroup();
}

void appPreferencesDialog::loadProfileString(QString profile, QString label)
{
  QSettings settings;
  QString value;

  settings.beginGroup("Profiles");
  settings.beginGroup(profile);
  value = settings.value(label).toString();
  settings.endGroup();
  settings.endGroup();

  settings.setValue( label, value ); 
}

void appPreferencesDialog::loadProfile()
{
  QString profile=QString("profile") + glob.profileId();

  loadProfileString( profile, "Name" );
  loadProfileString( profile, "default_channel_order" );
  loadProfileString( profile, "default_mode" );
  loadProfileString( profile, "burnFirmware" );
  loadProfileString( profile, "rename_firmware_files" );
  loadProfileString( profile, "sdPath" );
  loadProfileString( profile, "SplashFileName" );
  loadProfileString( profile, "firmware" );
}

void appPreferencesDialog::on_removeProfileButton_clicked()
{
  QSettings settings;
  if ( glob.profileId() == 1 )
     QMessageBox::information(this, tr("Not possible to remove profile"), tr("The default profile can not be removed."));
  else
  {
    QString profile=QString("profile") + glob.profileId();
    settings.beginGroup("Profiles");
    settings.remove(profile);
    settings.endGroup();
    settings.setValue("profileId", "1");

    loadProfile();
    initSettings();
  }
}

bool appPreferencesDialog::displayImage( QString fileName )
{
  QImage image(fileName);
  if (image.isNull()) 
    return false;

// This code below just figures out if the width of the latest firmware is 128 or 212. It works , but...
  QString filePath1 = glob.lastFlashDir() + "/" + glob.firmware() + ".bin";
  QString filePath2 = glob.lastFlashDir() + "/" + glob.firmware() + ".hex";
  QFile file(filePath1);
  if (!file.exists())
    filePath1 = filePath2;
  int width = SPLASH_WIDTH;
  FlashInterface flash(filePath1);
  if (flash.hasSplash())
    width = flash.getSplashWidth(); // Returns SPLASHX9D_HEIGHT if filePath1 does not exist!
// There must be a cleaner way of finding out the width of the firmware splash!

  ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(width, SPLASH_HEIGHT)));
  if (width==SPLASHX9D_WIDTH) {
    image=image.convertToFormat(QImage::Format_RGB32);
    QRgb col;
    int gray, height = image.height();
    for (int i = 0; i < width; ++i) {
      for (int j = 0; j < height; ++j) {
        col = image.pixel(i, j);
        gray = qGray(col);
        image.setPixel(i, j, qRgb(gray, gray, gray));
      }
    }      
    ui->imageLabel->setPixmap(QPixmap::fromImage(image));
  } 
  else {
    ui->imageLabel->setPixmap(QPixmap::fromImage(image.convertToFormat(QImage::Format_Mono)));
  }
  if (width == SPLASH_WIDTH)
      ui->imageLabel->setFixedSize(SPLASH_WIDTH, SPLASH_HEIGHT);
  else
     ui->imageLabel->setFixedSize(SPLASHX9D_WIDTH, SPLASHX9D_HEIGHT);

  return true;
}

void appPreferencesDialog::on_SplashSelect_clicked()
{
  QString supportedImageFormats;
  for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
    supportedImageFormats += QLatin1String(" *.") + QImageReader::supportedImageFormats()[formatIndex];
  }

  QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open Image to load"), glob.lastImagesDir(), tr("Images (%1)").arg(supportedImageFormats));

  if (!fileName.isEmpty()) {
    glob.lastImagesDir(QFileInfo(fileName).dir().absolutePath());
    
    if (displayImage(fileName))
      ui->SplashFileName->setText(fileName);
  }
}

void appPreferencesDialog::on_clearImageButton_clicked() {
  ui->imageLabel->clear();
  ui->SplashFileName->clear();
}



