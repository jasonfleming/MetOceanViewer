//-------------------------------GPL-------------------------------------//
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
// The name "MetOcean Viewer" is specific to this project and may not be
// used for projects "forked" or derived from this work.
//
//-----------------------------------------------------------------------//

#include <MetOceanViewer.h>
#include <ui_MetOceanViewer_main.h>
#include <netcdf.h>

//A variable used to define an alternate save location
QString AlternateFolder;

//-------------------------------------------//
//Routine used for saving a session in
//netCDF format. This will define the file
//locations in a relative way
//-------------------------------------------//
int MainWindow::saveSession()
{
    int ierr,ncid,i;
    int dimid_ntimeseries,dimid_one;
    int varid_filename,varid_colors,varid_units,varid_names;
    int varid_xshift,varid_yshift,varid_type,varid_coldstart;
    int varid_stationfile,varid_plottitle,varid_xlabel,varid_ylabel;
    int varid_startdate,varid_enddate,varid_precision,varid_ymin,varid_ymax;
    int varid_autodate,varid_autoy,varid_checkState;
    int dims_1d[1];
    int nTimeseries;
    QString relFile,relPath,TempFile,Directory;
    size_t start[1];
    size_t iu;
    double mydatadouble[1];
    int mydataint[1];
    const char * mydatastring[1];

    QFile Session(SessionFile);

    QVector<QString> filenames_ts;
    QVector<QString> filetype_ts;
    QVector<QString> colors_ts;
    QVector<double> units_ts;
    QVector<QString> seriesname_ts;
    QVector<double> xshift_ts;
    QVector<double> yshift_ts;
    QVector<QString> date_ts;
    QVector<QString> stationfile_ts;
    QVector<int> checkStates_ts;

    //See how we are approaching this routine. If "save as..." or "load" has not been clicked before
    //get a file name to save as.
    if(SessionFile==NULL)
    {
        QString SaveFile = QFileDialog::getSaveFileName(this,"Save Session...",
                                                        PreviousDirectory,
                                                        "MetOceanViewer Session (*.mvs)");
        if(SaveFile==NULL)
            return 0;
        else
            SessionFile = SaveFile;
    }

    //Remove the old file
    if(Session.exists())
        Session.remove();

    //Get the path of the session file so we can save a relative path later
    mov_generic::splitPath(SessionFile,TempFile,Directory);
    QDir CurrentDir(Directory);

    ierr = mov_generic::NETCDF_ERR(nc_create(SessionFile.toUtf8(),NC_NETCDF4,&ncid));
    if(ierr!=NC_NOERR)return 1;

    //Start setting up the definitions
    nTimeseries = ui->table_TimeseriesData->rowCount();

    filenames_ts.resize(nTimeseries);
    colors_ts.resize(nTimeseries);
    units_ts.resize(nTimeseries);
    seriesname_ts.resize(nTimeseries);
    xshift_ts.resize(nTimeseries);
    yshift_ts.resize(nTimeseries);
    date_ts.resize(nTimeseries);
    stationfile_ts.resize(nTimeseries);
    filetype_ts.resize(nTimeseries);
    checkStates_ts.resize(nTimeseries);

    for(i=0;i<nTimeseries;i++)
    {
        filenames_ts[i] = ui->table_TimeseriesData->item(i,6)->text();
        seriesname_ts[i] = ui->table_TimeseriesData->item(i,1)->text();
        colors_ts[i] = ui->table_TimeseriesData->item(i,2)->text();
        units_ts[i] = ui->table_TimeseriesData->item(i,3)->text().toDouble();
        xshift_ts[i] = ui->table_TimeseriesData->item(i,4)->text().toDouble();
        yshift_ts[i] = ui->table_TimeseriesData->item(i,5)->text().toDouble();
        date_ts[i] = ui->table_TimeseriesData->item(i,7)->text();
        filetype_ts[i] = ui->table_TimeseriesData->item(i,8)->text();
        stationfile_ts[i] = ui->table_TimeseriesData->item(i,10)->text();
        if(ui->table_TimeseriesData->item(i,0)->checkState()==Qt::Checked)
            checkStates_ts[i] = 1;
        else
            checkStates_ts[i] = 0;
    }

    ierr = mov_generic::NETCDF_ERR(nc_def_dim(ncid,"ntimeseries",static_cast<size_t>(nTimeseries),&dimid_ntimeseries));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_dim(ncid,"one",1,&dimid_one));
    if(ierr!=NC_NOERR)return 1;

    //Arrays
    dims_1d[0] = dimid_ntimeseries;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_filename",NC_STRING,1,dims_1d,&varid_filename));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_colors",NC_STRING,1,dims_1d,&varid_colors));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_names",NC_STRING,1,dims_1d,&varid_names));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_filetype",NC_STRING,1,dims_1d,&varid_type));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_coldstartdate",NC_STRING,1,dims_1d,&varid_coldstart));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_stationfile",NC_STRING,1,dims_1d,&varid_stationfile));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_xshift",NC_DOUBLE,1,dims_1d,&varid_xshift));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_yshift",NC_DOUBLE,1,dims_1d,&varid_yshift));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_units",NC_DOUBLE,1,dims_1d,&varid_units));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_checkState",NC_INT,1,dims_1d,&varid_checkState));
    if(ierr!=NC_NOERR)return 1;

    //Scalars
    dims_1d[0] = dimid_one;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_plottitle",NC_STRING,1,dims_1d,&varid_plottitle));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_xlabel",NC_STRING,1,dims_1d,&varid_xlabel));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_ylabel",NC_STRING,1,dims_1d,&varid_ylabel));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_precision",NC_INT,1,dims_1d,&varid_precision));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_startdate",NC_STRING,1,dims_1d,&varid_startdate));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_enddate",NC_STRING,1,dims_1d,&varid_enddate));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_ymin",NC_DOUBLE,1,dims_1d,&varid_ymin));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_ymax",NC_DOUBLE,1,dims_1d,&varid_ymax));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_autodate",NC_INT,1,dims_1d,&varid_autodate));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_def_var(ncid,"timeseries_autoy",NC_INT,1,dims_1d,&varid_autoy));
    if(ierr!=NC_NOERR)return 1;
    ierr = mov_generic::NETCDF_ERR(nc_enddef(ncid));
    if(ierr!=NC_NOERR)return 1;

    mydatastring[0] = ui->text_TimeseriesPlotTitle->text().toUtf8();
    ierr = mov_generic::NETCDF_ERR(nc_put_var_string(ncid,varid_plottitle,mydatastring));
    if(ierr!=NC_NOERR)return 1;
    mydatastring[0] = ui->text_TimeseriesXaxisLabel->text().toUtf8();
    ierr = mov_generic::NETCDF_ERR(nc_put_var_string(ncid,varid_xlabel,mydatastring));
    if(ierr!=NC_NOERR)return 1;
    mydatastring[0] = ui->text_TimeseriesYaxisLabel->text().toUtf8();
    ierr = mov_generic::NETCDF_ERR(nc_put_var_string(ncid,varid_ylabel,mydatastring));
    if(ierr!=NC_NOERR)return 1;
    mydatastring[0] = ui->date_TimeseriesStartDate->dateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8();
    ierr = mov_generic::NETCDF_ERR(nc_put_var_string(ncid,varid_startdate,mydatastring));
    if(ierr!=NC_NOERR)return 1;
    mydatastring[0] = ui->date_TimeseriesEndDate->dateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8();
    ierr = mov_generic::NETCDF_ERR(nc_put_var_string(ncid,varid_enddate,mydatastring));
    if(ierr!=NC_NOERR)return 1;
    mydataint[0] = 3;
    ierr = mov_generic::NETCDF_ERR(nc_put_var_int(ncid,varid_precision,mydataint));
    if(ierr!=NC_NOERR)return 1;
    mydatadouble[0] = ui->spin_TimeseriesYmin->value();
    ierr = mov_generic::NETCDF_ERR(nc_put_var_double(ncid,varid_ymin,mydatadouble));
    if(ierr!=NC_NOERR)return 1;
    mydatadouble[0] = ui->spin_TimeseriesYmax->value();
    ierr = mov_generic::NETCDF_ERR(nc_put_var_double(ncid,varid_ymax,mydatadouble));
    if(ierr!=NC_NOERR)return 1;

    if(ui->check_TimeseriesAllData->isChecked())
        mydataint[0] = 1;
    else
        mydataint[0] = 0;
    ierr = mov_generic::NETCDF_ERR(nc_put_var_int(ncid,varid_autodate,mydataint));
    if(ierr!=NC_NOERR)return 1;

    if(ui->check_TimeseriesYauto->isChecked())
        mydataint[0] = 1;
    else
        mydataint[0] = 0;
    ierr = mov_generic::NETCDF_ERR(nc_put_var_int(ncid,varid_autoy,mydataint));
    if(ierr!=NC_NOERR)return 1;

    for(iu=0;iu<static_cast<unsigned int>(nTimeseries);iu++)
    {
        start[0] = iu;

        relPath = CurrentDir.relativeFilePath(filenames_ts[iu]);
        mydatastring[0]  = relPath.toUtf8();
        ierr  = mov_generic::NETCDF_ERR(nc_put_var1_string(ncid,varid_filename,start,mydatastring));
        if(ierr!=NC_NOERR)return 1;

        mydatastring[0]  = seriesname_ts[iu].toUtf8();
        ierr  = mov_generic::NETCDF_ERR(nc_put_var1_string(ncid,varid_names,start,mydatastring));
        if(ierr!=NC_NOERR)return 1;

        mydatastring[0]  = colors_ts[iu].toUtf8();
        ierr  = mov_generic::NETCDF_ERR(nc_put_var1_string(ncid,varid_colors,start,mydatastring));
        if(ierr!=NC_NOERR)return 1;

        mydatastring[0]  = date_ts[iu].toUtf8();
        ierr  = mov_generic::NETCDF_ERR(nc_put_var1_string(ncid,varid_coldstart,start,mydatastring));
        if(ierr!=NC_NOERR)return 1;

        relPath = CurrentDir.relativeFilePath(stationfile_ts[iu]);
        mydatastring[0]  = relPath.toUtf8();
        ierr  = mov_generic::NETCDF_ERR(nc_put_var1_string(ncid,varid_stationfile,start,mydatastring));
        if(ierr!=NC_NOERR)return 1;

        mydatastring[0]  = filetype_ts[iu].toUtf8();
        ierr  = mov_generic::NETCDF_ERR(nc_put_var1_string(ncid,varid_type,start,mydatastring));
        if(ierr!=NC_NOERR)return 1;

        mydatadouble[0]  = xshift_ts[iu];
        ierr  = mov_generic::NETCDF_ERR(nc_put_var1_double(ncid,varid_xshift,start,mydatadouble));
        if(ierr!=NC_NOERR)return 1;

        mydatadouble[0]  = yshift_ts[iu];
        ierr  = mov_generic::NETCDF_ERR(nc_put_var1_double(ncid,varid_yshift,start,mydatadouble));
        if(ierr!=NC_NOERR)return 1;

        mydatadouble[0]  = units_ts[iu];
        ierr  = mov_generic::NETCDF_ERR(nc_put_var1_double(ncid,varid_units,start,mydatadouble));
        if(ierr!=NC_NOERR)return 1;

        mydataint[0] = checkStates_ts[iu];
        ierr = mov_generic::NETCDF_ERR(nc_put_var1_int(ncid,varid_checkState,start,mydataint));
        if(ierr!=NC_NOERR)return 1;

    }

    ierr = mov_generic::NETCDF_ERR(nc_close(ncid));
    if(ierr!=NC_NOERR)return 1;

    return 0;
}
//-------------------------------------------//


//-------------------------------------------//
//This routine will load the session file
//from netCDF format
//-------------------------------------------//
int MainWindow::loadSession()
{
    int ierr,ncid,i,nrow;
    int dimid_ntimeseries,nTimeseries;
    int varid_filename,varid_colors,varid_units,varid_names;
    int varid_xshift,varid_yshift,varid_type,varid_coldstart;
    int varid_stationfile,varid_plottitle,varid_xlabel,varid_ylabel;
    int varid_startdate,varid_enddate,varid_precision,varid_ymin,varid_ymax;
    int varid_autodate,varid_autoy,varid_checkState;
    const char * mydatachar[1];
    double mydatadouble[1];
    int mydataint[1];
    QMessageBox::StandardButton reply;
    QString filelocation,filename,series_name,color,type;
    QString coldstartstring,stationfile,stationfilepath;
    QString BaseFile,CurrentDirectory,NewFile,TempFile,BaseDir;
    double unitconvert,xshift,yshift;
    size_t temp_size_t;
    size_t start[1];
    QDate tempstartdate,tempenddate;
    QString tempstring;
    QColor CellColor;
    QDateTime ColdStart;
    //ADCNC NetCDFData;
    //ADCASCII ADCData;
    bool continueToLoad,hasCheckInfo;
    Qt::CheckState checkState;

    QFile Session(SessionFile);
    if(!Session.exists())
    {
        QMessageBox::critical(this,"ERROR","File not found.");
        return 1;
    }

    //Open the netCDF file
    ierr = mov_generic::NETCDF_ERR(nc_open(SessionFile.toUtf8(),NC_NOWRITE,&ncid));
    if(ierr!=NC_NOERR)return 1;

    //Read some of the basics from the file (dimensions, variable IDs)
    ierr = mov_generic::NETCDF_ERR(nc_inq_dimid(ncid,"ntimeseries",&dimid_ntimeseries));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_filename",&varid_filename));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_colors",&varid_colors));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_names",&varid_names));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_filetype",&varid_type));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_units",&varid_units));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_xshift",&varid_xshift));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_yshift",&varid_yshift));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_coldstartdate",&varid_coldstart));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_stationfile",&varid_stationfile));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_plottitle",&varid_plottitle));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_xlabel",&varid_xlabel));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_ylabel",&varid_ylabel));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_precision",&varid_precision));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_startdate",&varid_startdate));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_enddate",&varid_enddate));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_ymin",&varid_ymin));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_ymax",&varid_ymax));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_autodate",&varid_autodate));
    if(ierr!=NC_NOERR)return 1;

    ierr = mov_generic::NETCDF_ERR(nc_inq_varid(ncid,"timeseries_autoy",&varid_autoy));
    if(ierr!=NC_NOERR)return 1;

    ierr = nc_inq_varid(ncid,"timeseries_checkState",&varid_checkState);
    if(ierr!=NC_NOERR)
        hasCheckInfo = false;
    else
        hasCheckInfo = true;

    //Read the scalar variables
    ierr = mov_generic::NETCDF_ERR(nc_get_var(ncid,varid_plottitle,&mydatachar));
    if(ierr!=NC_NOERR)return 1;
    ui->text_TimeseriesPlotTitle->setText(QString(mydatachar[0]));

    ierr = mov_generic::NETCDF_ERR(nc_get_var(ncid,varid_xlabel,&mydatachar));
    if(ierr!=NC_NOERR)return 1;
    ui->text_TimeseriesXaxisLabel->setText(QString(mydatachar[0]));

    ierr = mov_generic::NETCDF_ERR(nc_get_var(ncid,varid_ylabel,&mydatachar));
    if(ierr!=NC_NOERR)return 1;
    ui->text_TimeseriesYaxisLabel->setText(QString(mydatachar[0]));

    ierr = mov_generic::NETCDF_ERR(nc_get_var(ncid,varid_startdate,&mydatachar));
    if(ierr!=NC_NOERR)return 1;
    tempstring = QString(mydatachar[0]);
    tempstartdate = QDateTime::fromString(tempstring,"yyyy-MM-dd hh:mm:ss").date();
    ui->date_TimeseriesStartDate->setDate(tempstartdate);

    ierr = mov_generic::NETCDF_ERR(nc_get_var(ncid,varid_enddate,&mydatachar));
    if(ierr!=NC_NOERR)return 1;
    tempstring = QString(mydatachar[0]);
    tempenddate = QDateTime::fromString(tempstring,"yyyy-MM-dd hh:mm:ss").date();
    ui->date_TimeseriesEndDate->setDate(tempenddate);

    ierr = mov_generic::NETCDF_ERR(nc_get_var(ncid,varid_ymin,&mydatadouble));
    if(ierr!=NC_NOERR)return 1;
    ui->spin_TimeseriesYmin->setValue(mydatadouble[0]);

    ierr = mov_generic::NETCDF_ERR(nc_get_var(ncid,varid_ymax,&mydatadouble));
    if(ierr!=NC_NOERR)return 1;
    ui->spin_TimeseriesYmax->setValue(mydatadouble[0]);

    ierr = mov_generic::NETCDF_ERR(nc_get_var(ncid,varid_autodate,&mydataint));
    if(ierr!=NC_NOERR)return 1;
    if(mydataint[0]==0)
        ui->check_TimeseriesAllData->setChecked(false);
    else
        ui->check_TimeseriesAllData->setChecked(true);

    ierr = mov_generic::NETCDF_ERR(nc_get_var(ncid,varid_autoy,&mydataint));
    if(ierr!=NC_NOERR)return 1;
    if(mydataint[0]==0)
        ui->check_TimeseriesYauto->setChecked(false);
    else
        ui->check_TimeseriesYauto->setChecked(true);

    //Now, before we begin building the table, we need to empty everything
//    for(i=TimeseriesData.length()-1;i>=0;i--)
//    {
//        TimeseriesData.remove(i);
//        ui->table_TimeseriesData->removeRow(i);
//    }

    //Next, read in the data and add rows to the table
    ierr = mov_generic::NETCDF_ERR(nc_inq_dimlen(ncid,dimid_ntimeseries,&temp_size_t));
    if(ierr!=NC_NOERR)return 1;
    nTimeseries = static_cast<int>(temp_size_t);
    nrow = 0;

    //Get the location we are currently working in
    mov_generic::splitPath(SessionFile,TempFile,CurrentDirectory);

    for(i=0;i<nTimeseries;i++)
    {
        start[0] = static_cast<size_t>(i);

        ierr = mov_generic::NETCDF_ERR(nc_get_var1(ncid,varid_filename,start,&mydatachar));
        if(ierr!=NC_NOERR)return 1;
        filelocation = QString(mydatachar[0]);
        filename = TempFile;

        ierr = mov_generic::NETCDF_ERR(nc_get_var1(ncid,varid_names,start,&mydatachar));
        if(ierr!=NC_NOERR)return 1;
        series_name = QString(mydatachar[0]);

        ierr = mov_generic::NETCDF_ERR(nc_get_var1(ncid,varid_type,start,&mydatachar));
        if(ierr!=NC_NOERR)return 1;
        type = QString(mydatachar[0]);

        ierr = mov_generic::NETCDF_ERR(nc_get_var1(ncid,varid_colors,start,&mydatachar));
        if(ierr!=NC_NOERR)return 1;
        color = QString(mydatachar[0]);

        ierr = mov_generic::NETCDF_ERR(nc_get_var1(ncid,varid_units,start,&mydatadouble));
        if(ierr!=NC_NOERR)return 1;
        unitconvert = mydatadouble[0];

        ierr = mov_generic::NETCDF_ERR(nc_get_var1(ncid,varid_coldstart,start,&mydatachar));
        if(ierr!=NC_NOERR)return 1;
        coldstartstring = QString(mydatachar[0]);

        ierr = mov_generic::NETCDF_ERR(nc_get_var1(ncid,varid_xshift,start,&mydatadouble));
        if(ierr!=NC_NOERR)return 1;
        xshift = mydatadouble[0];

        ierr = mov_generic::NETCDF_ERR(nc_get_var1(ncid,varid_yshift,start,&mydatadouble));
        if(ierr!=NC_NOERR)return 1;
        yshift = mydatadouble[0];

        ierr = mov_generic::NETCDF_ERR(nc_get_var1(ncid,varid_stationfile,start,&mydatachar));
        if(ierr!=NC_NOERR)return 1;
        stationfilepath = QString(mydatachar[0]);
        mov_generic::splitPath(stationfilepath,stationfile,TempFile);

        if(hasCheckInfo)
        {
            ierr = mov_generic::NETCDF_ERR(nc_get_var1(ncid,varid_checkState,start,&mydataint));
            if(mydataint[0]==1)
                checkState = Qt::Checked;
            else
                checkState = Qt::Unchecked;
        }
        else
            checkState = Qt::Checked;

        continueToLoad = false;

        filelocation = CurrentDirectory+"/"+filelocation;
        mov_generic::splitPath(filelocation,BaseFile,BaseDir);

        QFile myfile(filelocation);
        if(!myfile.exists())
        {
            //The file wasn't found where we think it should be. Give the
            //user a chance to specify an alternate data directory
            if(AlternateFolder==NULL)
            {
                //If we haven't previously specified an alternate folder, inform the user and
                //ask if they want to specify.
                reply = QMessageBox::question(this,"File not found",
                    "Data file not found in default location. Would you like to specify another?");
                if(reply==QMessageBox::Yes)
                {
                    //Get an alternate location
                    AlternateFolder = QFileDialog::getExistingDirectory(this,"Select Folder",PreviousDirectory);
                    NewFile = AlternateFolder+"/"+BaseFile;
                    QFile myfile(NewFile);
                    if(!myfile.exists())
                    {
                        continueToLoad = false;
                        QMessageBox::critical(this,"File Not Found","The file "+
                                              BaseFile+" was not found and has been skipped.");
                    }
                    else
                    {
                        continueToLoad = true;
                        filelocation = NewFile;
                    }
                }
                else
                    continueToLoad = false;
            }
            else
            {
                //Start by trying the previously specified alternate folder
                NewFile = AlternateFolder+"/"+BaseFile;
                QFile myfile(NewFile);
                if(!myfile.exists())
                {
                    reply = QMessageBox::question(this,"File not found",
                        "File not found in default location. Would you like to specify another?");
                    if(reply==QMessageBox::Yes)
                    {
                        //Get an alternate location
                        AlternateFolder = QFileDialog::getExistingDirectory(this,"Select Folder",PreviousDirectory);
                        NewFile = AlternateFolder+"/"+BaseFile;
                        QFile myfile(NewFile);
                        if(!myfile.exists())
                        {
                            continueToLoad = false;
                            QMessageBox::critical(this,"File Not Found","The file "+
                                                  BaseFile+" was not found and has been skipped.");
                        }
                        else
                        {
                            continueToLoad = true;
                            filelocation = NewFile;
                        }
                    }
                    else
                        continueToLoad = false;
                }
                else
                {
                    continueToLoad = true;
                    filelocation = NewFile;
                }
            }
        }
        else
        {
            continueToLoad = true;
        }

        if(type == "ADCIRC")
        {
            mov_generic::splitPath(stationfilepath,BaseFile,BaseDir);
            stationfilepath = CurrentDirectory+"/"+stationfilepath;
            QFile myfile(stationfilepath);
            if(!myfile.exists())
            {
                //The file wasn't found where we think it should be. Give the
                //user a chance to specify an alternate data directory
                if(AlternateFolder==NULL)
                {
                    //If we haven't previously specified an alternate folder, inform the user and
                    //ask if they want to specify.
                    reply = QMessageBox::question(this,"File not found",
                        "Station file not found in default location. Would you like to specify another?");
                    if(reply==QMessageBox::Yes)
                    {
                        //Get an alternate location
                        AlternateFolder = QFileDialog::getExistingDirectory(this,"Select Folder",PreviousDirectory);
                        NewFile = AlternateFolder+"/"+BaseFile;
                        QFile myfile(NewFile);
                        if(!myfile.exists())
                        {
                            continueToLoad = false;
                            QMessageBox::critical(this,"File Not Found","The file "+
                                                  BaseFile+" was not found and has been skipped.");
                        }
                        else
                        {
                            continueToLoad = true;
                            stationfilepath = NewFile;
                        }
                    }
                    else
                        continueToLoad = false;
                }
                else
                {
                    //Start by trying the previously specified alternate folder
                    NewFile = AlternateFolder+"/"+BaseFile;
                    QFile myfile(NewFile);
                    if(!myfile.exists())
                    {
                        reply = QMessageBox::question(this,"File not found",
                            "File not found in default location. Would you like to specify another?");
                        if(reply==QMessageBox::Yes)
                        {
                            //Get an alternate location
                            AlternateFolder = QFileDialog::getExistingDirectory(this,"Select Folder",PreviousDirectory);
                            NewFile = AlternateFolder+"/"+BaseFile;
                            QFile myfile(NewFile);
                            if(!myfile.exists())
                            {
                                continueToLoad = false;
                                QMessageBox::critical(this,"File Not Found","The file "+
                                                      BaseFile+" was not found and has been skipped.");
                            }
                            else
                            {
                                continueToLoad = true;
                                stationfilepath = NewFile;
                            }
                        }
                        else
                            continueToLoad = false;
                    }
                    else
                    {
                        continueToLoad = true;
                        stationfilepath = NewFile;
                    }
                }
            }
            else
            {
                continueToLoad = true;
            }
        }

        if(continueToLoad)
        {
            //Build the table
            nrow = nrow + 1;
            ui->table_TimeseriesData->setRowCount(nrow);
            ui->table_TimeseriesData->setRowCount(nrow);
            ui->table_TimeseriesData->setItem(nrow-1,0,new QTableWidgetItem(filename));
            ui->table_TimeseriesData->setItem(nrow-1,1,new QTableWidgetItem(series_name));
            ui->table_TimeseriesData->setItem(nrow-1,2,new QTableWidgetItem(color));
            ui->table_TimeseriesData->setItem(nrow-1,3,new QTableWidgetItem(QString::number(unitconvert)));
            ui->table_TimeseriesData->setItem(nrow-1,4,new QTableWidgetItem(QString::number(xshift)));
            ui->table_TimeseriesData->setItem(nrow-1,5,new QTableWidgetItem(QString::number(yshift)));
            ui->table_TimeseriesData->setItem(nrow-1,6,new QTableWidgetItem(filelocation));
            ui->table_TimeseriesData->setItem(nrow-1,7,new QTableWidgetItem((coldstartstring)));
            ui->table_TimeseriesData->setItem(nrow-1,8,new QTableWidgetItem(type));
            ui->table_TimeseriesData->setItem(nrow-1,9,new QTableWidgetItem(stationfile));
            ui->table_TimeseriesData->setItem(nrow-1,10,new QTableWidgetItem(stationfilepath));
            CellColor.setNamedColor(color);
            ui->table_TimeseriesData->item(nrow-1,2)->setBackgroundColor(CellColor);
            ui->table_TimeseriesData->item(nrow-1,0)->setCheckState(checkState);
            ColdStart = QDateTime::fromString(coldstartstring,"yyyy-MM-dd hh:mm:ss");

            //Read the data into the appropriate structure
            //TimeseriesData.resize(nrow);

            //TimeseriesData[nrow-1].success = false;

//            if(type=="IMEDS")
//            {
//                TimeseriesData[nrow-1] = readIMEDS(filelocation);
//            }
//            else if(type=="NETCDF")
//            {
//                ierr = readADCIRCnetCDF(filelocation,NetCDFData);
//                if(!NetCDFData.success)
//                    TimeseriesData[nrow-1].success = false;
//                else
//                {
//                    TimeseriesData[nrow-1] = NetCDF_to_IMEDS(NetCDFData,ColdStart);
//                }
//            }
//            else if(type=="ADCIRC")
//            {
//                ADCData = readADCIRCascii(filelocation,stationfilepath);
//                if(!ADCData.success)
//                    TimeseriesData[nrow-1].success = false;
//                else
//                {
//                    TimeseriesData[nrow-1] = ADCIRC_to_IMEDS(ADCData,ColdStart);
//                }
//            }

        }
    }
    ierr = mov_generic::NETCDF_ERR(nc_close(ncid));
    if(ierr!=NC_NOERR)return 1;

    //Change to the time series data screen
    ui->MainTabs->setCurrentIndex(1);
    ui->subtab_timeseries->setCurrentIndex(0);

    return 0;
}
//-------------------------------------------//
