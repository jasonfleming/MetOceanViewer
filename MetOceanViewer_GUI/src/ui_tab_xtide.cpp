/*-------------------------------GPL-------------------------------------//
//
// MetOcean Viewer - A simple interface for viewing hydrodynamic model data
// Copyright (C) 2015  Zach Cobell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------*/
#include "MetOceanViewer.h"
#include "ui_MetOceanViewer_main.h"
#include "xtide.h"

void MainWindow::on_button_xtide_compute_clicked()
{
    this->plotXTideStation();
    return;
}


void MainWindow::plotXTideStation()
{
    int ierr;

    //...Create an xTide object
    if(!this->thisXTide.isNull())
        delete this->thisXTide;
    this->thisXTide = new XTide(ui->xtide_map,ui->xtide_graphics,ui->date_xtide_start,
                                ui->date_xtide_end,ui->combo_xtide_units,ui->statusBar,this);
    connect(thisXTide,SIGNAL(xTideError(QString)),this,SLOT(throwErrorMessageBox(QString)));

    //...Call the plotting routine
    ierr = this->thisXTide->plotXTideStation();

    return;

}

void MainWindow::on_button_xtide_resetzoom_clicked()
{
    if(!this->thisXTide.isNull())
        ui->xtide_graphics->resetZoom();
    return;
}


void MainWindow::on_combo_xtide_panto_activated(const QString &arg1)
{
    ui->xtide_map->page()->runJavaScript("panTo('"+ui->combo_xtide_panto->currentText()+"')");
    return;
}


void MainWindow::on_button_xtide_savemap_clicked()
{
    QString filename;

    QString MarkerID = thisXTide->getLoadedXTideStation();
    QString MarkerID2 = thisXTide->getCurrentXTideStation();

    if(MarkerID=="none")
    {
        QMessageBox::critical(this,"ERROR","No Station has been selected.");
        return;
    }

    if(MarkerID != MarkerID2)
    {
        QMessageBox::critical(this,"ERROR","The currently selected station is not the data loaded.");
        return;
    }

    QString filter = "JPG (*.jpg *.jpeg)";
    QString DefaultFile = "/XTide_"+MarkerID.replace(" ","_")+".jpg";
    QString TempString = QFileDialog::getSaveFileName(this,"Save as...",
                PreviousDirectory+DefaultFile,"JPG (*.jpg *.jpeg) ;; PDF (*.pdf)",&filter);

    if(TempString==NULL)
        return;

    mov_generic::splitPath(TempString,filename,PreviousDirectory);

    thisXTide->saveXTidePlot(TempString,filter);

    return;
}

void MainWindow::on_button_xtide_savedata_clicked()
{
    QString filename;

    QString MarkerID = thisXTide->getLoadedXTideStation();
    QString MarkerID2 = thisXTide->getCurrentXTideStation();

    if(MarkerID=="none")
    {
        QMessageBox::critical(this,"ERROR","No Station has been selected.");
        return;
    }

    if(MarkerID != MarkerID2)
    {
        QMessageBox::critical(this,"ERROR","The currently selected station is not the data loaded.");
        return;
    }

    QString filter;
    QString DefaultFile = "/XTide_"+MarkerID.replace(" ","_")+".imeds";

    QString TempString = QFileDialog::getSaveFileName(this,"Save as...",
                                    PreviousDirectory+DefaultFile,
                                    "IMEDS (*.imeds);;CSV (*.csv)",&filter);

    QStringList filter2 = filter.split(" ");
    QString format = filter2.value(0);

    if(TempString == NULL)
        return;

    mov_generic::splitPath(TempString,filename,PreviousDirectory);

    thisXTide->saveXTideData(TempString,format);

    return;
}
