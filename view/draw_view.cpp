// --------------------------------------------------------------------------------
/*
    draw_view.cpp

  Copyright (c) 2009-2015  Thomas Mueller, Frank Grave


   This file is part of the GeodesicViewer.

   The GeodesicViewer is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   The GeodesicViewer is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the GeodesicViewer.  If not, see <http://www.gnu.org/licenses/>.
*/
// -------------------------------------------------------------------------------

#include <QColorDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QTimer>

#include "draw_view.h"

extern m4d::Object mObject;

/*! Standard constructor.
 *  \param  data : pointer to Object storing all m4d-values.
 *  \param  opengl : pointer to opengl-model.
 *  \param  draw   : pointer to 2d-canvas.
 *  \param  oglJacobi : pointer to opengl-jacobi-model.
 *  \param  par : struct to parameters.
 *  \param  parent : pointer to parent widget.
 */
DrawView::DrawView(OpenGL3dModel* opengl, OpenGL2dModel* draw, OpenGLJacobiModel* oglJacobi, struct_params* par, QWidget* parent)
    : QWidget(parent) {
    mOpenGL = opengl;
    mDraw   = draw;

    assert(oglJacobi != NULL);
    mOglJacobi = oglJacobi;

    mParams = par;

    mBGcolor     = mParams->opengl_bg_color;
    mFGcolor     = mParams->opengl_line_color;
    mEmbColor    = Qt::lightGray;

    m2dFGcolor   = mParams->draw2d_line_color;
    m2dBGcolor   = mParams->draw2d_bg_color;
    m2dGridColor = mParams->draw2d_grid_color;

    init();
}

/*! Standard destructor.
 */
DrawView::~DrawView() {
}


// *********************************** public methods *********************************

/*!
 */
void DrawView::resetAll() {
    cob_mousehandle->setCurrentIndex(0);
    mOpenGL->setMouseHandle((enum_mouse_handle)0);

    led_draw2d_x_min->setValueAndStep(DEF_DRAW2D_X_INIT_MIN, DEF_DRAW2D_X_STEP);
    led_draw2d_y_min->setValueAndStep(DEF_DRAW2D_Y_INIT_MIN, DEF_DRAW2D_Y_STEP);
    led_draw2d_x_max->setValueAndStep(DEF_DRAW2D_X_INIT_MAX, DEF_DRAW2D_X_STEP);
    led_draw2d_y_max->setValueAndStep(DEF_DRAW2D_Y_INIT_MAX, DEF_DRAW2D_Y_STEP);

    cob_drawtype->clear();
    cob_drawtype->addItem(QString(m4d::stl_draw_type[m4d::enum_draw_coordinates]));
    cob_abscissa->setEnabled(false);
    cob_ordinate->setEnabled(false);

    m2dFGcolor   = mParams->draw2d_line_color;
    m2dBGcolor   = mParams->draw2d_bg_color;
    m2dGridColor = mParams->draw2d_grid_color;

    pub_draw2d_bgcolor->setPalette(QPalette(m2dBGcolor));
    pub_draw2d_fgcolor->setPalette(QPalette(m2dFGcolor));
    pub_draw2d_gridcolor->setPalette(QPalette(m2dGridColor));
    spb_draw2d_linewidth->setValue(1);

    cob_projection->setCurrentIndex(mParams->opengl_projection);
    int dt = cob_drawtype3d->findText(m4d::stl_draw_type[(int)mParams->opengl_draw3d_type]);
    cob_drawtype3d->setCurrentIndex(dt);
    cob_view->setCurrentIndex(0);

    led_fov->setValueAndStep(mParams->opengl_fov, mParams->opengl_fov_step);
    m4d::vec3 eye = mParams->opengl_eye_pos;
    m4d::vec3 poi = mParams->opengl_eye_poi;
    led_eye_x->setValueAndStep(eye[0], mParams->opengl_eye_pos_step);
    led_eye_y->setValueAndStep(eye[1], mParams->opengl_eye_pos_step);
    led_eye_z->setValueAndStep(eye[2], mParams->opengl_eye_pos_step);
    led_poi_x->setValueAndStep(poi[0], mParams->opengl_eye_poi_step);
    led_poi_y->setValueAndStep(poi[1], mParams->opengl_eye_poi_step);
    led_poi_z->setValueAndStep(poi[2], mParams->opengl_eye_poi_step);

    mBGcolor  = mParams->opengl_bg_color;
    mFGcolor  = mParams->opengl_line_color;
    mEmbColor = Qt::lightGray;
    pub_bgcolor->setPalette(QPalette(mBGcolor));
    pub_fgcolor->setPalette(QPalette(mFGcolor));
    pub_emb_color->setPalette(QPalette(mEmbColor));
    mOpenGL->setColors(mFGcolor, mBGcolor, mEmbColor);

    spb_linewidth->setValue(mParams->opengl_line_width);

    led_scale3d_x->setValueAndStep(mParams->opengl_scale_x, DEF_DRAW3D_SCALE_X_STEP);
    led_scale3d_y->setValueAndStep(mParams->opengl_scale_x, DEF_DRAW3D_SCALE_Y_STEP);
    led_scale3d_z->setValueAndStep(mParams->opengl_scale_x, DEF_DRAW3D_SCALE_Z_STEP);
    led_scale3d_x_step->setValueAndStep(DEF_DRAW3D_SCALE_X_STEP, DEF_DRAW3D_SCALE_X_STEP * 0.1);
    led_scale3d_y_step->setValueAndStep(DEF_DRAW3D_SCALE_Y_STEP, DEF_DRAW3D_SCALE_Y_STEP * 0.1);
    led_scale3d_z_step->setValueAndStep(DEF_DRAW3D_SCALE_Z_STEP, DEF_DRAW3D_SCALE_Z_STEP * 0.1);

    chb_stereo->setCheckState(Qt::Unchecked);
    cob_glasses->setCurrentIndex((int)mParams->opengl_stereo_glasses);
    cob_stereo_type->setCurrentIndex((int)mParams->opengl_stereo_type);

    chb_fog->setCheckState(Qt::Unchecked);
    led_fog_density->setValueAndStep(mParams->opengl_fog_init, mParams->opengl_fog_step);
    led_fog_density_step->setValueAndStep(mParams->opengl_fog_step, mParams->opengl_fog_step * 0.1);

    chb_anim_localrot->setChecked(false);
    led_anim_rotate_x->setValueAndStep(DEF_OPENGL_ANIM_ROT_X_INIT, 0.01);
    led_anim_rotate_y->setValueAndStep(DEF_OPENGL_ANIM_ROT_Y_INIT, 0.01);
    led_anim_rotate_z->setValueAndStep(DEF_OPENGL_ANIM_ROT_Z_INIT, 0.01);
    led_anim_rotate_x_step->setValueAndStep(0.01, 0.001);
    led_anim_rotate_y_step->setValueAndStep(0.01, 0.001);
    led_anim_rotate_z_step->setValueAndStep(0.01, 0.001);
    pub_anim_rotate->setChecked(false);
    mOpenGL->setAnimRotationParams(mParams->opengl_anim_rot_x, mParams->opengl_anim_rot_y, mParams->opengl_anim_rot_z);

    sli_anim_geodlength->setValue(0);
    sli_anim_geodlength->setMaximum(0);
    led_lastpoint_affineparam->setText(QString());
    for (int i = 0; i < lst_led_lastpoint_pos_value.size(); i++) {
        lst_led_lastpoint_pos_value[i]->setText(QString());
    }

    adjustEmbParams();
    pub_emb_color->setPalette(QPalette(mParams->opengl_emb_color));

    slot_resetScaling();
}

/*!
 */
void DrawView::updateData() {
}

/*!
 */
void DrawView::updateParams() {
    /* --- 2d stuff --- */
    led_draw2d_x_min->setValue(mParams->draw2d_xMin);
    led_draw2d_x_max->setValue(mParams->draw2d_xMax);
    led_draw2d_y_min->setValue(mParams->draw2d_yMin);
    led_draw2d_y_max->setValue(mParams->draw2d_yMax);
    mDraw->setScaling(mParams->draw2d_xMin, mParams->draw2d_xMax, mParams->draw2d_yMin, mParams->draw2d_yMax);

    m2dBGcolor   = mParams->draw2d_bg_color;
    m2dFGcolor   = mParams->draw2d_line_color;
    m2dGridColor = mParams->draw2d_grid_color;

    pub_draw2d_bgcolor->setPalette(m2dBGcolor);
    pub_draw2d_fgcolor->setPalette(m2dFGcolor);
    pub_draw2d_gridcolor->setPalette(m2dGridColor);
    //pub_draw2d_zoomcolor->setPalette( m2dZoomColor );
    spb_draw2d_linewidth->setValue(mParams->draw2d_line_width);
    mDraw->setColors(m2dFGcolor, m2dBGcolor, m2dGridColor);

    //if (mParams->draw2d_representation>=0 && mParams->draw2d_representation<cob_drawtype->count())
    //  cob_drawtype->setCurrentIndex(mParams->draw2d_representation);

    if (mParams->draw2d_representation >= 0 && mParams->draw2d_representation < m4d::NUM_ENUM_DRAW_TYPE) {
        for (int i = 0; i < cob_drawtype->count(); i++) {
            // std::cerr << cob_drawtype->itemText(i).toStdString() << " " << m4d::stl_draw_type[mParams->draw2d_representation] << std::endl;
            if (cob_drawtype->itemText(i).toStdString() == m4d::stl_draw_type[mParams->draw2d_representation]) {
                cob_drawtype->setCurrentIndex(i);
                break;
            }
        }
    }

    if (mParams->draw2d_abscissa < 0) {
        mParams->draw2d_abscissa = 0;
    }
    if (mParams->draw2d_abscissa >= cob_abscissa->count()) {
        mParams->draw2d_abscissa = cob_abscissa->count() - 1;
    }
    cob_abscissa->setCurrentIndex(mParams->draw2d_abscissa);

    if (mParams->draw2d_ordinate < 0) {
        mParams->draw2d_ordinate = 0;
    }
    if (mParams->draw2d_ordinate >= cob_ordinate->count()) {
        mParams->draw2d_ordinate = cob_ordinate->count() - 1;
    }
    cob_ordinate->setCurrentIndex(mParams->draw2d_ordinate);

    if (mObject.currMetric == NULL) {
        return;
    }

    m4d::enum_draw_type  type = mObject.currMetric->getCurrDrawType(cob_drawtype->currentIndex());
    if (type == m4d::enum_draw_coordinates) {
        cob_abscissa->setEnabled(true);
        cob_ordinate->setEnabled(true);
    } else {
        cob_abscissa->setEnabled(false);
        cob_ordinate->setEnabled(false);
    }

    cob_projection->setCurrentIndex(mParams->opengl_projection);
    int dt = cob_drawtype3d->findText(m4d::stl_draw_type[(int)mParams->opengl_draw3d_type]);
    cob_drawtype3d->setCurrentIndex(dt);

    led_fov->setValueAndStep(mParams->opengl_fov, mParams->opengl_fov_step);
    m4d::vec3 eye = mParams->opengl_eye_pos;
    m4d::vec3 poi = mParams->opengl_eye_poi;
    led_eye_x->setValueAndStep(eye[0], mParams->opengl_eye_pos_step);
    led_eye_y->setValueAndStep(eye[1], mParams->opengl_eye_pos_step);
    led_eye_z->setValueAndStep(eye[2], mParams->opengl_eye_pos_step);
    led_poi_x->setValueAndStep(poi[0], mParams->opengl_eye_poi_step);
    led_poi_y->setValueAndStep(poi[1], mParams->opengl_eye_poi_step);
    led_poi_z->setValueAndStep(poi[2], mParams->opengl_eye_poi_step);

    /* --- colors --- */
    mBGcolor  = mParams->opengl_bg_color;
    mFGcolor  = mParams->opengl_line_color;
    mEmbColor = mParams->opengl_emb_color;
    pub_bgcolor->setPalette(QPalette(mBGcolor));
    pub_fgcolor->setPalette(QPalette(mFGcolor));
    pub_emb_color->setPalette(QPalette(mEmbColor));
    mOpenGL->setColors(mFGcolor, mBGcolor, mEmbColor);

    spb_linewidth->setValue(mParams->opengl_line_width);
    if (mParams->opengl_line_smooth == 1) {
        chb_linesmooth->setChecked(true);
    } else {
        chb_linesmooth->setChecked(false);
    }

    /* --- embedding --- */
    if (mParams->opengl_emb_params.size() > 0 && mObject.currMetric != NULL) {
        std::map<std::string, double>::iterator mapItr = mParams->opengl_emb_params.begin();
        while (mapItr != mParams->opengl_emb_params.end()) {
            mObject.currMetric->setEmbeddingParam(mapItr->first, mapItr->second);
            mapItr++;
        }
    }
    adjustEmbParams();
    if (mParams->opengl_draw3d_type == m4d::enum_draw_embedding) {
        mOpenGL->genEmbed(mObject.currMetric);
    } else {
        mOpenGL->clearEmbed();
    }

    /* --- stereo --- */
    if (mParams->opengl_stereo_use == 1) {
        chb_stereo->setCheckState(Qt::Checked);
    } else {
        chb_stereo->setCheckState(Qt::Unchecked);
    }
    cob_glasses->setCurrentIndex((int)mParams->opengl_stereo_glasses);
    cob_stereo_type->setCurrentIndex((int)mParams->opengl_stereo_type);

    led_eye_sep->setValueAndStep(mParams->opengl_stereo_sep, mParams->opengl_stereo_step);
    led_eye_sep_step->setValueAndStep(mParams->opengl_stereo_step, mParams->opengl_stereo_step * 0.1);

    /* --- fog --- */
    if (mParams->opengl_fog_use == 1) {
        chb_fog->setCheckState(Qt::Checked);
    } else {
        chb_fog->setCheckState(Qt::Unchecked);
    }
    led_fog_density->setValueAndStep(mParams->opengl_fog_init, mParams->opengl_fog_step);
    led_fog_density_step->setValueAndStep(mParams->opengl_fog_step, mParams->opengl_fog_step * 0.1);

    /* --- 3d-scaling --- */
    led_scale3d_x->setValue(mParams->opengl_scale_x);
    led_scale3d_y->setValue(mParams->opengl_scale_y);
    led_scale3d_z->setValue(mParams->opengl_scale_z);

    /* --- 3d-anim --- */
    if (mParams->opengl_anim_local > 0) {
        chb_anim_localrot->setChecked(true);
    } else {
        chb_anim_localrot->setChecked(false);
    }
    led_anim_rotate_x->setValue(mParams->opengl_anim_rot_x);
    led_anim_rotate_y->setValue(mParams->opengl_anim_rot_y);
    led_anim_rotate_z->setValue(mParams->opengl_anim_rot_z);
    mOpenGL->setAnimRotationParams(mParams->opengl_anim_rot_x, mParams->opengl_anim_rot_y, mParams->opengl_anim_rot_z);
}

/*! Adjust coordinate names.
 *  Read the coordinate names of the current metric and write them into the comboboxes 'cob_abscissa' and 'cob_ordinate' for 2d plot.
 */
void DrawView::adjustCoordNames() {
    if (mObject.currMetric == NULL) {
        return;
    }

    std::string coordNames[4];
    mObject.currMetric->getCoordNames(coordNames);

    cob_abscissa->clear();
    cob_ordinate->clear();
    cob_abscissa->setEnabled(false);
    cob_ordinate->setEnabled(false);
    mDraw->setAbsOrd(enum_draw_coord_x0, enum_draw_coord_x0);

    for (int i = 0; i < 4; i++) {
        QChar ch = mGreekLetter.toChar(coordNames[i].c_str());
        if (ch == QChar()) {
            cob_abscissa->addItem(coordNames[i].c_str());
            cob_ordinate->addItem(coordNames[i].c_str());
            lst_lab_lastpoint_coordname[i]->setText(coordNames[i].c_str());
        } else {
            cob_abscissa->addItem(QString(ch));
            cob_ordinate->addItem(QString(ch));
            lst_lab_lastpoint_coordname[i]->setText(QString(ch));
        }
    }
    QChar lch = mGreekLetter.toChar("lambda");
    cob_abscissa->addItem(QString(lch));
    cob_ordinate->addItem(QString(lch));
    for (int i = 0; i < 4; i++) {
        QChar ch = mGreekLetter.toChar(coordNames[i].c_str());
        if (ch == QChar()) {
            cob_abscissa->addItem(QString("d") + coordNames[i].c_str());
            cob_ordinate->addItem(QString("d") + coordNames[i].c_str());
        } else {
            cob_abscissa->addItem(QString("d") + QString(ch));
            cob_ordinate->addItem(QString("d") + QString(ch));
        }
    }

    slot_adjustAPname();
}

/*! Adjust drawing types.
 */
void DrawView::adjustDrawTypes() {
    if (mObject.currMetric == NULL) {
        return;
    }

    std::vector<m4d::enum_draw_type> drawtypes;
    int numDT = mObject.currMetric->getDrawTypes(drawtypes);
    cob_drawtype->clear();
    cob_drawtype3d->clear();
    for (int i = 0; i < numDT; i++) {
        m4d::enum_draw_type  type = mObject.currMetric->getCurrDrawType(i);
        if (type == m4d::enum_draw_pseudocart || type == m4d::enum_draw_coordinates || type == m4d::enum_draw_effpoti || type == m4d::enum_draw_custom) {
            cob_drawtype->addItem(QString(m4d::stl_draw_type[type]));
        }

        if (type != m4d::enum_draw_coordinates && type != m4d::enum_draw_effpoti) {
            cob_drawtype3d->addItem(QString(m4d::stl_draw_type[type]));
        }
    }
}

/*! Adjust embedding parameters.
 */
void DrawView::adjustEmbParams() {
    for (int i = tbw_emb_params->rowCount(); i >= 0; --i) {
        tbw_emb_params->removeRow(0);
    }

    if (mObject.currMetric == NULL) {
        return;
    }

    /* --- set embedding parameter names --- */
    std::vector<std::string> paramNames;
    mObject.currMetric->getEmbeddingNames(paramNames);

    for (unsigned int i = 0; i < paramNames.size(); i++) {
        tbw_emb_params->insertRow(i);
        QTableWidgetItem* item_name = new QTableWidgetItem();
        item_name->setText(paramNames[i].c_str());
        item_name->setFlags(Qt::ItemIsEnabled);
        tbw_emb_params->setItem(i, 0, item_name);

        double value;
        mObject.currMetric->getEmbeddingParam(paramNames[i], value);
        DoubleEdit* led_value = new DoubleEdit(DEF_PREC_EMBEDDING, value, 1.0);
        led_value->setObjectName(paramNames[i].c_str());

        DoubleEdit* led_step = new DoubleEdit(DEF_PREC_EMBEDDING, 1.0, 0.1);

        tbw_emb_params->setCellWidget(i, 1, led_value);
        tbw_emb_params->setCellWidget(i, 2, led_step);
        tbw_emb_params->setRowHeight(i, DEF_MAXIMUM_ELEM_HEIGHT);

        connect(led_value, SIGNAL(editingFinished()), this, SLOT(slot_embParamChanged()));
        connect(led_step,  SIGNAL(editingFinished()), led_value, SLOT(slot_setStep()));
    }
    tbw_emb_params->resizeColumnsToContents();
}

/*!
 */
void DrawView::adjustLastPoint(unsigned int num) {
    m4d::vec4 last_point;
    double    last_affine;

    if (mObject.points.size() == 0) {
        return;
    }

    if (num < mObject.points.size()) {
        last_point  = mObject.points[num];
        if (num >= mObject.maxNumPoints) {
            last_point = mObject.points[mObject.maxNumPoints - 1];
        }

        for (int i = 0; i < 4; i++) {
            lst_led_lastpoint_pos_value[i]->setText(QString::number(last_point[i], 'f', DEF_PREC_POSITION));
        }
    }

    if (num < mObject.lambda.size()) {
        last_affine = mObject.lambda[num];
        if (num >= mObject.maxNumPoints) {
            last_affine = mObject.lambda[mObject.maxNumPoints - 1];
        }

        led_lastpoint_affineparam->setText(QString::number(last_affine, 'f', DEF_PREC_POSITION));
    }
}

/*! Get combobox index of drawing type.
 *  \return int : index of the current 2d drawing type.
 */
int
DrawView::getDrawTypeIndex() {
    return cob_drawtype->currentIndex();
}

/*! Get combobox index of 3d drawing type.
 *  \return int : index of the current 3d drawing type.
 */
int
DrawView::getDrawType3DIndex() {
    return cob_drawtype3d->currentIndex();
}

/*! Get name of drawing type.
 *  \return string : name of the current 2d drawing type.
 */
std::string
DrawView ::getDrawTypeName() {
    return cob_drawtype->currentText().toStdString();
}

void DrawView::getDrawAbsOrdIndex(int &absIdx, int &ordIdx) {
    absIdx = cob_abscissa->currentIndex();
    ordIdx = cob_ordinate->currentIndex();
}

/*! Get name of 3d drawing type.
 *  \return string :  name of the current 3d drawing type.
 */
std::string
DrawView ::getDrawType3DName() {
    return cob_drawtype3d->currentText().toStdString();
}

/*! Get background and foreground colors.
 *  \param bgcol : reference to background color.
 *  \param fgcol : reference to foreground color.
 *  \param embcol : reference to embedding color.
 */
void DrawView::getColors(QColor &bgcol, QColor &fgcol, QColor &embcol) {
    bgcol = mBGcolor;
    fgcol = mFGcolor;
    embcol = mEmbColor;
}

/*!
 *  \param anim : set pushbutton for rotation animation.
 */
void DrawView::animate(bool anim) {
    pub_anim_rotate->setChecked(anim);
}

/*!
 *  \return true : do animation.
 */
bool DrawView::isAnimated() {
    return pub_anim_rotate->isChecked();
}

/*! Set number of segments of the geodesic.
 */
void DrawView::setGeodLength(int num) {
    sli_anim_geodlength->setMaximum(num);
    sli_anim_geodlength->setValue(num);
    adjustLastPoint(num);
}


void DrawView::addObjectsToScriptEngine(QScriptEngine* engine) {
    QScriptValue drawView = engine->newQObject(this);
    engine->globalObject().setProperty("dh", drawView);

    QScriptValue sliAnimGeodLength = engine->newQObject(sli_anim_geodlength);
    engine->globalObject().setProperty("ShowPoints", sliAnimGeodLength);
}

// ************************************* public slots ***********************************
/*! Set draw3d type.
 *  \param num : number of draw type.
 */
void DrawView::setType(int num) {
    if (num >= 0 && num < cob_drawtype3d->count()) {
        cob_drawtype3d->setCurrentIndex(num);
        slot_setDrawType3d();
    }
}

/*! Set camera position.
 * \param x : x coordinate.
 * \param y : y coordinate.
 * \param z : z coordinate.
 */
void DrawView::setPosition(double x, double y, double z) {
    m4d::vec3 dp = m4d::vec3(x, y, z) - mOpenGL->getCameraPoi();
    double dist = dp.getNorm();

    double theta = atan2(sqrt(dp.x(0) * dp.x(0) + dp.x(1) * dp.x(1)), dp.x(2));
    double phi   = atan2(dp.x(1), dp.x(0));

    led_eye_x->setValue(x);
    led_eye_y->setValue(y);
    led_eye_z->setValue(z);
    mOpenGL->setCameraSphere(theta, phi, dist);
    slot_setCameraPos();
}

/*! Set point of interest.
 * \param x : x coordinate.
 * \param y : y coordinate.
 * \param z : z coordinate.
 */
void DrawView::setPOI(double x, double y, double z) {
    led_poi_x->setValue(x);
    led_poi_y->setValue(y);
    led_poi_z->setValue(z);
    slot_setCameraPoi();
}

/*! Set field of view.
 * \param fov : field of view in degree
 */
void DrawView::setFoV(double fov) {
    led_fov->setValue(fov);
    slot_setFieldOfView();
}

/*! Set scaling.
 * \param sx : scale in x direction.
 * \param sy : scale in y direction.
 * \param sz : scale in z direction.
 */
void DrawView::setScale(double sx, double sy, double sz) {
    led_scale3d_x->setValue(sx);
    led_scale3d_y->setValue(sy);
    led_scale3d_z->setValue(sz);
    slot_set3dScaling();
}

/*! Set artificial fog.
 * \param fog : enable/disable fog
 * \param density : fog density
 */
void DrawView::setFog(int fog, double density) {
    if (fog > 0) {
        chb_fog->setChecked(true);
    } else {
        chb_fog->setChecked(false);
    }
    led_fog_density->setValue(density);
    slot_setFog();
    slot_setFogDensity();
}

/*! Set number of points that shall be shown.
 * \param num : number of points.
 */
void DrawView::showNumPoints(int num) {
    if (num >= 0 && num < sli_anim_geodlength->maximum()) {
        sli_anim_geodlength->setValue(num);
        lcd_anim_geodlength->display(num);
        slot_showNumPoints(num);
    }
}

// *********************************** protected slots **********************************
/*! Set draw type.
 *  The drawing type could be e.g. pseudo-cartesian or embedding. Compare 'enum_draw_type'
 *  of the Motion4D-library.
 */
void DrawView::slot_setDrawType() {
    if (mObject.currMetric == NULL) {
        return;
    }

    //m4d::enum_draw_type  type = mObject.currMetric->getCurrDrawType(cob_drawtype->currentIndex());
    //mParams->draw2d_representation = (int)type;
    m4d::enum_draw_type  type = mObject.currMetric->getCurrDrawType(cob_drawtype->currentText().toStdString());
    mParams->draw2d_representation = type;

    if (type == m4d::enum_draw_coordinates) {
        cob_abscissa->setEnabled(true);
        cob_ordinate->setEnabled(true);
    } else {
        cob_abscissa->setEnabled(false);
        cob_ordinate->setEnabled(false);
    }
    emit calcGeodesic();
}

/*! Set abscissa and ordinate for coorinate plotting in 2d.
 *  Readout combobox entries cob_abscissa and cob_ordinate.
 */
void DrawView::slot_setAbsOrd() {
    int abscissa = cob_abscissa->currentIndex();
    int ordinate = cob_ordinate->currentIndex();
    mDraw->setAbsOrd((enum_draw_coord_num)abscissa, (enum_draw_coord_num)ordinate);
    mDraw->setPoints(mObject.currMetric->getCurrDrawType(cob_drawtype->currentIndex()));
    mParams->draw2d_abscissa = abscissa;
    mParams->draw2d_ordinate = ordinate;
}

/*! Set scaling for 2d plot.
 *  Readout lineedit entries led_scale_x_min,...
 */
void DrawView::slot_setScaling() {
    double  xmin = led_draw2d_x_min->getValue();
    double  xmax = led_draw2d_x_max->getValue();
    double  ymin = led_draw2d_y_min->getValue();
    double  ymax = led_draw2d_y_max->getValue();

    mDraw->setScaling(xmin, xmax, ymin, ymax);
}

/*! Reset scaling for 2d plot.
 *  Set the scaling parameters to the standard definitions.
 *  \sa gdefs.h
 */
void DrawView::slot_resetScaling() {
    double aspect = (DEF_OPENGL_HEIGHT - DEF_DRAW2D_BOTTOM_BORDER) / (double)(DEF_OPENGL_WIDTH - DEF_DRAW2D_LEFT_BORDER);
    led_draw2d_x_min->setValue(DEF_DRAW2D_X_INIT_MIN);
    led_draw2d_x_max->setValue(DEF_DRAW2D_X_INIT_MAX);
    led_draw2d_y_min->setValue(-(DEF_DRAW2D_X_INIT_MAX - DEF_DRAW2D_X_INIT_MIN) * 0.5 * aspect);
    led_draw2d_y_max->setValue((DEF_DRAW2D_X_INIT_MAX - DEF_DRAW2D_X_INIT_MIN) * 0.5 * aspect);

    mDraw->setScaling(DEF_DRAW2D_X_INIT_MIN, DEF_DRAW2D_X_INIT_MAX, -(DEF_DRAW2D_X_INIT_MAX - DEF_DRAW2D_X_INIT_MIN) * 0.5 * aspect, (DEF_DRAW2D_X_INIT_MAX - DEF_DRAW2D_X_INIT_MIN) * 0.5 * aspect);
    mDraw->setStepIdx(DEF_DRAW2D_X_STEP, DEF_DRAW2D_Y_STEP);
}

void DrawView::slot_centerScaling() {
    mDraw->center();
    double xmin, xmax, ymin, ymax;
    mDraw->getScaling(xmin, xmax, ymin, ymax);
    led_draw2d_x_min->setValue(xmin);
    led_draw2d_x_max->setValue(xmax);
    led_draw2d_y_min->setValue(xmin);
    led_draw2d_y_max->setValue(xmax);
}

/*! Adjust scaling for 2d plot.
 *  Read the scaling parameters of the 2d-model and write them into the lineedits.
 *  \sa Draw2dModel.
 */
void DrawView::slot_adjustScaling() {
    double xmin, xmax, ymin, ymax;
    mDraw->getScaling(xmin, xmax, ymin, ymax);

    led_draw2d_x_min->setValue(xmin);
    led_draw2d_x_max->setValue(xmax);
    led_draw2d_y_min->setValue(ymin);
    led_draw2d_y_max->setValue(ymax);

    double x, y;
    mDraw->getCurrPos(x, y);

    led_draw2d_currX->setText(QString::number(x));
    led_draw2d_currY->setText(QString::number(y));
}


/*! Zoom for 2d plot.
 */
void DrawView::slot_zoom() {
    double  xmin = led_draw2d_x_min->getValue();
    double  xmax = led_draw2d_x_max->getValue();
    double  ymin = led_draw2d_y_min->getValue();
    double  ymax = led_draw2d_y_max->getValue();

    QObject* obj = sender();
    if (obj->objectName() == QString("pub_zoom_in")) {
        xmin += DEF_ZOOM_STEP;
        xmax -= DEF_ZOOM_STEP;
        ymin += DEF_ZOOM_STEP;
        ymax -= DEF_ZOOM_STEP;
    } else {
        xmin -= DEF_ZOOM_STEP;
        xmax += DEF_ZOOM_STEP;
        ymin -= DEF_ZOOM_STEP;
        ymax += DEF_ZOOM_STEP;
    }
    mDraw->setScaling(xmin, xmax, ymin, ymax);
    led_draw2d_x_min->setValue(xmin);
    led_draw2d_x_max->setValue(xmax);
    led_draw2d_y_min->setValue(ymin);
    led_draw2d_y_max->setValue(ymax);
}

/*!
 */
void DrawView::slot_set2dFGcolor() {
    QColor  fgcol = QColorDialog::getColor(m2dFGcolor);

    if (fgcol.isValid()) {
        m2dFGcolor = fgcol;
        mParams->draw2d_line_color = fgcol;
        pub_draw2d_fgcolor->setPalette(QPalette(m2dFGcolor));
        mDraw->setFGcolor(fgcol);
    }
}

/*!
 */
void DrawView::slot_set2dBGcolor() {
    QColor  bgcol = QColorDialog::getColor(m2dBGcolor);

    if (bgcol.isValid()) {
        m2dBGcolor = bgcol;
        mParams->draw2d_bg_color = bgcol;
        pub_draw2d_bgcolor->setPalette(QPalette(m2dBGcolor));
        mDraw->setBGcolor(bgcol);
    }
}

/*!
 */
void DrawView::slot_set2dGridColor() {
    QColor  gridcol = QColorDialog::getColor(m2dGridColor);

    if (gridcol.isValid()) {
        m2dGridColor = gridcol;
        mParams->draw2d_grid_color = gridcol;
        pub_draw2d_gridcolor->setPalette(QPalette(m2dGridColor));
        mDraw->setGridColor(gridcol);
    }
}

/*!
 */
void DrawView::slot_set2dLineWidth() {
    mParams->draw2d_line_width = spb_draw2d_linewidth->value();
    mDraw->setLineWidth(mParams->draw2d_line_width);
}

/*! Set mouse handle.
 *  Readout the combobox 'cob_mousehandle' and set the mouse handling for 3d.
 *  \sa OpenGL3dModel.
 */
void DrawView::slot_setMouseHandle() {
    enum_mouse_handle  handle =  enum_mouse_handle(cob_mousehandle->currentIndex());
    mOpenGL->setMouseHandle(handle);
}

/*! Set 3d projection type.
 *  Readout the combobox 'cob_projection' and set the projection mode for 3d.
 *  \sa OpenGL3dModel.
 */
void DrawView::slot_setProjection() {
    enum_projection  proj = enum_projection(cob_projection->currentIndex());
    mOpenGL->setProjection(proj);
    mParams->opengl_projection = (int)proj;
}

/*! Set field of view for 3d.
 *  Set the field of view for 3d.
 *  \sa OpenGL3dModel.
 */
void DrawView::slot_setFieldOfView() {
    mOpenGL->setFieldOfView(led_fov->getValue());
    mParams->opengl_fov = led_fov->getValue();
}

/*! Adjust the camera parameters.
 *  Read the camera position and point of interest from the opengl-model.
 */
void DrawView::slot_adjustCamera() {
    mOpenGL->getCameraDirs(mParams->opengl_eye_pos, mParams->opengl_eye_poi, mParams->opengl_eye_vup);
    led_eye_x->setValue(mParams->opengl_eye_pos[0]);
    led_eye_y->setValue(mParams->opengl_eye_pos[1]);
    led_eye_z->setValue(mParams->opengl_eye_pos[2]);
    led_poi_x->setValue(mParams->opengl_eye_poi[0]);
    led_poi_y->setValue(mParams->opengl_eye_poi[1]);
    led_poi_z->setValue(mParams->opengl_eye_poi[2]);
    double fov = mOpenGL->getFieldOfView();
    led_fov->setValue(fov);
}

/*! Set the camera position.
 *  Readout the lineedit entries and set the opengl camera.
 */
void DrawView::slot_setCameraPos() {
    mParams->opengl_eye_pos = m4d::vec3(led_eye_x->getValue(), led_eye_y->getValue(), led_eye_z->getValue());
    mOpenGL->setCameraPos(mParams->opengl_eye_pos);
}

/*! Set the point of interest.
 *  Readout the lineedit entries and set the point of interest.
 */
void DrawView::slot_setCameraPoi() {
    mParams->opengl_eye_poi = m4d::vec3(led_poi_x->getValue(), led_poi_y->getValue(), led_poi_z->getValue());
    mOpenGL->setCameraPoi(mParams->opengl_eye_poi);
}

/*! Set the predefined configurations for the camera.
 *  Readout the combobox 'cob_view' and the predefined camera parameters in the opengl-model.
 */
void DrawView::slot_setCameraPredefs() {
    enum_camera_predefs type = enum_camera_predefs(cob_view->currentIndex());
    mOpenGL->setCameraPredefs(type);
    slot_adjustCamera();
}

/*! rRset point of interest.
 */
void DrawView::slot_resetPoi() {
    led_poi_x->setValue(0.0);
    led_poi_y->setValue(0.0);
    led_poi_z->setValue(0.0);
    mOpenGL->setCameraPoi(m4d::vec3(0.0, 0.0, 0.0));
}

/*! Set draw type for 3d.
 */
void DrawView::slot_setDrawType3d() {
    if (mObject.currMetric == NULL) {
        return;
    }

    m4d::enum_draw_type  type = mObject.currMetric->getCurrDrawType(cob_drawtype3d->currentText().toStdString());
    mParams->opengl_draw3d_type = type;

    if (type == m4d::enum_draw_embedding) {
        mOpenGL->genEmbed(mObject.currMetric);
    } else {
        mOpenGL->clearEmbed();
    }

    emit calcGeodesic();
}

/*! Set foreground color.
 *  Set the line color of the geodesic in the opengl-model.
 */
void DrawView::slot_setFGcolor() {
    QColor  fgcol = QColorDialog::getColor(mFGcolor);

    if (fgcol.isValid()) {
        mFGcolor = fgcol;
        mParams->opengl_line_color = fgcol;
        pub_fgcolor->setPalette(QPalette(mFGcolor));
        emit colorChanged();
    }
}

/*! Set background color.
 */
void DrawView::slot_setBGcolor() {
    QColor  bgcol = QColorDialog::getColor(mBGcolor);

    if (bgcol.isValid()) {
        mBGcolor = bgcol;
        mParams->opengl_bg_color = bgcol;
        pub_bgcolor->setPalette(QPalette(mBGcolor));
        emit colorChanged();
    }
}

/*! Set embedding color.
 */
void DrawView::slot_setEmbColor() {
    QColor embcol = QColorDialog::getColor(mEmbColor);

    if (embcol.isValid()) {
        mParams->opengl_emb_color = mEmbColor = embcol;
        pub_emb_color->setPalette(QPalette(mEmbColor));
        emit colorChanged();
    }
}

/*! Set line with of geodesic.
 */
void DrawView::slot_setLineWidth() {
    mParams->opengl_line_width = spb_linewidth->value();
    mOpenGL->setLineWidth(mParams->opengl_line_width);
}

/*! Set line anti-aliasing.
 */
void DrawView::slot_setSmoothLine() {
    if (chb_linesmooth->isChecked()) {
        mParams->opengl_line_smooth = 1;
    } else {
        mParams->opengl_line_smooth = 0;
    }

    mOpenGL->setLineSmooth(mParams->opengl_line_smooth);
    // mDraw->setLineSmooth(mParams->opengl_line_smooth);
}

/*! Set scaling factors for 3d.
 */
void DrawView::slot_set3dScaling() {
    mParams->opengl_scale_x = led_scale3d_x->getValue();
    mParams->opengl_scale_y = led_scale3d_y->getValue();
    mParams->opengl_scale_z = led_scale3d_z->getValue();
    mOpenGL->setScaling(mParams->opengl_scale_x, mParams->opengl_scale_y, mParams->opengl_scale_z);
}

/*! Set scaling factor stepsizes for 3d.
 */
void DrawView::slot_set3dScalingStep() {
    led_scale3d_x->setStep(led_scale3d_x_step->getValue());
    led_scale3d_y->setStep(led_scale3d_y_step->getValue());
    led_scale3d_z->setStep(led_scale3d_z_step->getValue());
}

/*! Reset scaling factors.
 */
void DrawView::slot_set3dScalingReset() {
    mParams->opengl_scale_x = mParams->opengl_scale_y = mParams->opengl_scale_z = 1.0;
    led_scale3d_x->setValue(mParams->opengl_scale_x);
    led_scale3d_y->setValue(mParams->opengl_scale_y);
    led_scale3d_z->setValue(mParams->opengl_scale_z);
    mOpenGL->setScaling(mParams->opengl_scale_x, mParams->opengl_scale_y, mParams->opengl_scale_z);
}

/*! Toggle stereo.
 */
void DrawView::slot_setStereo() {
    if (chb_stereo->checkState() == Qt::Checked) {
        mOpenGL->setStereoCam(true);
        cob_glasses->setEnabled(true);
        cob_stereo_type->setEnabled(true);
        led_eye_sep->setEnabled(true);
        led_eye_sep_step->setEnabled(true);

        mParams->opengl_stereo_use = 1;
    } else {
        mOpenGL->setStereoCam(false);
        cob_glasses->setEnabled(false);
        cob_stereo_type->setEnabled(false);
        led_eye_sep->setEnabled(false);
        led_eye_sep_step->setEnabled(false);

        mParams->opengl_stereo_use = 0;
    }
}

/*! Set stereo parameters.
 */
void DrawView::slot_setStereoParams() {
    enum_stereo_glasses type = enum_stereo_glasses(cob_glasses->currentIndex());
    double sep = led_eye_sep->getValue();

    mOpenGL->setStereoParams(sep, type);
    mParams->opengl_stereo_sep = sep;
}

/*! Set stereo stepsize.
 */
void DrawView::slot_setStereoStep() {
    led_eye_sep->setStep(led_eye_sep_step->getValue());
}

void DrawView ::slot_setStereoType() {
    enum_stereo_type  type = enum_stereo_type(cob_stereo_type->currentIndex());
    mOpenGL->setStereoType(type);
}

/*! Toggle fog.
 */
void DrawView::slot_setFog() {
    if (chb_fog->checkState() == Qt::Checked) {
        mOpenGL->setFog(true);
        led_fog_density->setEnabled(true);
        led_fog_density_step->setEnabled(true);

        mParams->opengl_fog_use = 1;
    } else {
        mOpenGL->setFog(false);
        led_fog_density->setEnabled(false);
        led_fog_density_step->setEnabled(false);

        mParams->opengl_fog_use = 0;
    }
}

/*! Set fog density.
 */
void DrawView::slot_setFogDensity() {
    mOpenGL->setFogDensity(led_fog_density->getValue());
    mParams->opengl_fog_init = led_fog_density->getValue();
}

/*! Set stepsize for fog density.
 */
void DrawView::slot_setFogDensityStep() {
    led_fog_density->setStep(led_fog_density_step->getValue());
    mParams->opengl_fog_step = led_fog_density_step->getValue();
}

/*! Start or stop the animation.
 */
void DrawView::slot_anim_rot_startstop() {
    if (pub_anim_rotate->isChecked()) {
        tim_anim_rotate->start();
        pub_anim_rotate->setText("Stop");
    } else {
        tim_anim_rotate->stop();
        pub_anim_rotate->setText("Play");

        // set camera eye when animation stops.
        m4d::vec3 pos = mOpenGL->getCameraPos();
        led_eye_x->setValue(pos[0]);
        led_eye_y->setValue(pos[1]);
        led_eye_z->setValue(pos[2]);
    }
}

/*! Do rotation animation.
 *  Call the rotation animation of the opengl-model. Do a rotation with respect to the local tetrad axes or the coordinate axes.
 */
void DrawView::slot_anim_rotate() {
    bool local = chb_anim_localrot->isChecked();
    if (local) {
        mParams->opengl_anim_local = 1;
    } else {
        mParams->opengl_anim_local = 0;
    }
    mOpenGL->doAnimRotation(30.0, local);
    /*
    m4d::vec3 pos = mOpenGL->getCameraPos();
    led_eye_x->setValue(pos[0]);
    led_eye_y->setValue(pos[1]);
    led_eye_z->setValue(pos[2]);
    */
}

/*! Set parameters for the rotation animation.
 *  Readout the lineedits and set the rotation parameters for 3d.
 */
void DrawView::slot_setAnimRotateParams() {
    mParams->opengl_anim_rot_x = led_anim_rotate_x->getValue();
    mParams->opengl_anim_rot_y = led_anim_rotate_y->getValue();
    mParams->opengl_anim_rot_z = led_anim_rotate_z->getValue();
    mOpenGL->setAnimRotationParams(mParams->opengl_anim_rot_x, mParams->opengl_anim_rot_y, mParams->opengl_anim_rot_z);
}

/*! Set parameter stepsizes for the rotation animation.
 */
void DrawView::slot_setAnimRotateParamsStep() {
    led_anim_rotate_x->setStep(led_anim_rotate_x_step->getValue());
    led_anim_rotate_y->setStep(led_anim_rotate_y_step->getValue());
    led_anim_rotate_z->setStep(led_anim_rotate_z_step->getValue());
}

/*!
 *
 */
void DrawView::slot_embParamChanged() {
    QObject* obj = sender();
    QLineEdit* led = reinterpret_cast<QLineEdit*>(obj);
    double value = led->text().toDouble();

    if (mObject.currMetric != NULL) {
        mObject.currMetric->setEmbeddingParam(obj->objectName().toStdString(), value);

        std::map<std::string, double>::iterator mapItr = mParams->opengl_emb_params.find(obj->objectName().toStdString());
        if (mapItr != mParams->opengl_emb_params.end()) {
            mapItr->second = value;
        }
        slot_setDrawType3d();
    }
}

/*! SLOT: adjust affine parameter name.
 */
void DrawView::slot_adjustAPname() {
    QChar chAffine;
    if (mObject.type == m4d::enum_geodesic_timelike) {
        chAffine = mGreekLetter.toChar("tau");
    } else {
        chAffine = mGreekLetter.toChar("lambda");
    }
    lab_lastpoint_affineparam->setText(chAffine);
}

/*! Set number of points that shall be shown.
 * \param num : number of points.
 */
void DrawView::slot_showNumPoints(int num) {
    mOpenGL->showNumVerts(num);
    mDraw->showNumVerts(num);
    mOglJacobi->showNumJacobi(num);
    adjustLastPoint(num);
    emit lastPointChanged(num);
}

// *********************************** protected methods *********************************

/*! Initialize view.
 */
void DrawView::init() {
    initElements();
    initGUI();
    initControl();

    initStatusTips();
}

/*! Initialize view elements.
 */
void DrawView::initElements() {
    tab_draw = new QTabWidget();
    wgt_draw_2d = new QWidget();
    wgt_draw_3d = new QWidget();
    wgt_draw_3dscale  = new QWidget();
    wgt_draw_3dstereo = new QWidget();
    wgt_draw_3danim   = new QWidget();
    wgt_draw_3demb    = new QWidget();


    /* ---------------------------------
    *    2-D
    * --------------------------------- */
    cob_drawtype = new QComboBox();
    cob_drawtype->addItem(QString(m4d::stl_draw_type[m4d::enum_draw_coordinates]));
    cob_drawtype->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);


    lab_abscissa = new QLabel("abscissa");
    cob_abscissa = new QComboBox();
    cob_abscissa->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    cob_abscissa->setEnabled(false);
    lab_ordinate = new QLabel("ordinate");
    cob_ordinate = new QComboBox();
    cob_ordinate->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    cob_ordinate->setEnabled(false);

    lab_min = new QLabel("min");
    lab_min->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    lab_max = new QLabel("max");
    lab_max->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    lab_draw2d_abs = new QLabel("abscissa");
    lab_draw2d_abs->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_draw2d_x_min = new DoubleEdit(DEF_PREC_SCALE, DEF_DRAW2D_X_INIT_MIN, DEF_DRAW2D_X_STEP);
    led_draw2d_x_min->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_draw2d_x_max = new DoubleEdit(DEF_PREC_SCALE, DEF_DRAW2D_X_INIT_MAX, DEF_DRAW2D_X_STEP);
    led_draw2d_x_max->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    lab_draw2d_ord = new QLabel("ordinate");
    lab_draw2d_ord->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_draw2d_y_min = new DoubleEdit(DEF_PREC_SCALE, DEF_DRAW2D_Y_INIT_MIN, DEF_DRAW2D_Y_STEP);
    led_draw2d_y_min->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_draw2d_y_max = new DoubleEdit(DEF_PREC_SCALE, DEF_DRAW2D_Y_INIT_MAX, DEF_DRAW2D_Y_STEP);
    led_draw2d_y_max->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    pub_draw2d_center = new QPushButton("center");
    pub_draw2d_center->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    pub_draw2d_reset = new QPushButton("reset");
    pub_draw2d_reset->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    pub_draw2d_zoom_in  = new QPushButton("zoom in");
    pub_draw2d_zoom_in->setObjectName(QString("pub_zoom_in"));
    pub_draw2d_zoom_in->setAutoRepeat(true);
    pub_draw2d_zoom_in->setAutoRepeatDelay(400);
    pub_draw2d_zoom_in->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    pub_draw2d_zoom_out = new QPushButton("zoom out");
    pub_draw2d_zoom_out->setObjectName(QString("pub_zoom_out"));
    pub_draw2d_zoom_out->setAutoRepeat(true);
    pub_draw2d_zoom_out->setAutoRepeatDelay(400);
    pub_draw2d_zoom_out->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);


    lab_draw2d_bgcolor = new QLabel("bg color");
    lab_draw2d_bgcolor->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    pub_draw2d_bgcolor = new QPushButton();
    pub_draw2d_bgcolor->setPalette(QPalette(QColor(DEF_DRAW2D_BG_COLOR)));
    pub_draw2d_bgcolor->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    lab_draw2d_fgcolor = new QLabel("line color");
    lab_draw2d_fgcolor->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    pub_draw2d_fgcolor = new QPushButton();
    pub_draw2d_fgcolor->setPalette(QPalette(QColor(DEF_DRAW2D_LINE_COLOR)));
    pub_draw2d_fgcolor->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    lab_draw2d_gridcolor = new QLabel("grid color");
    lab_draw2d_gridcolor->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    pub_draw2d_gridcolor = new QPushButton();
    pub_draw2d_gridcolor->setPalette(QPalette(QColor(DEF_DRAW2D_GRID_COLOR)));
    pub_draw2d_gridcolor->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    lab_draw2d_linewidth = new QLabel("line width");
    lab_draw2d_linewidth->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    spb_draw2d_linewidth = new QSpinBox();
    spb_draw2d_linewidth->setRange(1, 5);
    spb_draw2d_linewidth->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    lab_draw2d_pos = new QLabel("Cur.pos.");
    led_draw2d_currX = new QLineEdit();
    led_draw2d_currX->setReadOnly(true);
    led_draw2d_currX->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    led_draw2d_currY = new QLineEdit();
    led_draw2d_currY->setReadOnly(true);
    led_draw2d_currY->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    /* ---------------------------------
    *    3-D
    * --------------------------------- */
    lab_projection = new QLabel("projection");
    cob_projection = new QComboBox();
    cob_projection->addItems(stl_projection);
    cob_projection->setMaximumSize(DEF_MAXIMUM_LE_W_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    cob_projection->setCurrentIndex(mParams->opengl_projection);

    lab_view = new QLabel("view");
    cob_view = new QComboBox();
    cob_view->addItems(stl_camera_predefs);
    cob_view->setMaximumSize(DEF_MAXIMUM_LE_W_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);

    lab_fov = new QLabel("FoV");
    lab_fov->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_fov = new DoubleEdit(DEF_PREC_DRAW, mParams->opengl_fov, mParams->opengl_fov_step);
    led_fov->setRange(1.0, 170.0);
    led_fov->setMaximumSize(DEF_MAXIMUM_LE_W_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);

    m4d::vec3 eye = mParams->opengl_eye_pos;
    lab_eye_x = new QLabel("eye x");
    lab_eye_x->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_eye_x = new DoubleEdit(DEF_PREC_DRAW, eye[0], mParams->opengl_eye_pos_step);
    led_eye_x->setMaximumSize(DEF_MAXIMUM_LE_W_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);

    lab_eye_y = new QLabel("eye y");
    lab_eye_y->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_eye_y = new DoubleEdit(DEF_PREC_DRAW, eye[1], mParams->opengl_eye_pos_step);
    led_eye_y->setMaximumSize(DEF_MAXIMUM_LE_W_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);

    lab_eye_z = new QLabel("eye z");
    lab_eye_z->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_eye_z = new DoubleEdit(DEF_PREC_DRAW, eye[2], mParams->opengl_eye_pos_step);
    led_eye_z->setMaximumSize(DEF_MAXIMUM_LE_W_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);

    lab_poi_x = new QLabel("poi x");
    lab_poi_x->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_poi_x = new DoubleEdit(DEF_PREC_DRAW, mParams->opengl_eye_poi[0], mParams->opengl_eye_poi_step);
    led_poi_x->setMaximumSize(DEF_MAXIMUM_LE_W_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);

    lab_poi_y = new QLabel("poi y");
    lab_poi_y->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_poi_y = new DoubleEdit(DEF_PREC_DRAW, mParams->opengl_eye_poi[1], mParams->opengl_eye_poi_step);
    led_poi_y->setMaximumSize(DEF_MAXIMUM_LE_W_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);

    lab_poi_z = new QLabel("poi z");
    lab_poi_z->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_poi_z = new DoubleEdit(DEF_PREC_DRAW, mParams->opengl_eye_poi[2], mParams->opengl_eye_poi_step);
    led_poi_z->setMaximumSize(DEF_MAXIMUM_LE_W_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);

    pub_reset_poi = new QPushButton("reset poi");
    pub_reset_poi->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    lab_bgcolor = new QLabel("bg color");
    lab_bgcolor->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    pub_bgcolor = new QPushButton();
    pub_bgcolor->setPalette(QPalette(Qt::black));
    pub_bgcolor->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    lab_fgcolor = new QLabel("line color");
    lab_fgcolor->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    pub_fgcolor = new QPushButton();
    pub_fgcolor->setPalette(QPalette(Qt::yellow));
    pub_fgcolor->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    lab_linewidth = new QLabel("line width");
    lab_linewidth->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    spb_linewidth = new QSpinBox();
    spb_linewidth->setRange(1, 5);
    spb_linewidth->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    chb_linesmooth = new QCheckBox("line smooth");
    chb_linesmooth->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    lab_drawtype3d = new QLabel("type");
    lab_drawtype3d->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    cob_drawtype3d = new QComboBox();
    cob_drawtype3d->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    cob_drawtype3d->setCurrentIndex(mParams->opengl_draw3d_type);

    /* ---------------------------------
    *    3-D  scale
    * --------------------------------- */
    lab_scale3d_value = new QLabel("value");
    lab_scale3d_step  = new QLabel("step");
    lab_scale3d_x = new QLabel("scale x");
    lab_scale3d_x->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_scale3d_x = new DoubleEdit(DEF_PREC_SCALE, mParams->opengl_scale_x, DEF_DRAW3D_SCALE_X_STEP);
    led_scale3d_x->setValidator(new QDoubleValidator(led_scale3d_x));
    led_scale3d_x->setRange(0.001, 10.0);
    led_scale3d_x->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    led_scale3d_x_step = new DoubleEdit(DEF_PREC_SCALE, 0.1, DEF_DRAW3D_SCALE_X_STEP * 0.1);
    led_scale3d_x_step->setValidator(new QDoubleValidator(led_scale3d_x_step));
    led_scale3d_x_step->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    lab_scale3d_y = new QLabel("scale y");
    lab_scale3d_y->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_scale3d_y = new DoubleEdit(DEF_PREC_SCALE, mParams->opengl_scale_y, DEF_DRAW3D_SCALE_Y_STEP);
    led_scale3d_y->setValidator(new QDoubleValidator(led_scale3d_y));
    led_scale3d_y->setRange(0.001, 10.0);
    led_scale3d_y->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    led_scale3d_y_step = new DoubleEdit(DEF_PREC_SCALE, 0.1, DEF_DRAW3D_SCALE_Y_STEP * 0.1);
    led_scale3d_y_step->setValidator(new QDoubleValidator(led_scale3d_y_step));
    led_scale3d_y_step->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    lab_scale3d_z = new QLabel("scale z");
    lab_scale3d_z->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_scale3d_z = new DoubleEdit(DEF_PREC_SCALE, mParams->opengl_scale_z, DEF_DRAW3D_SCALE_Z_STEP);
    led_scale3d_z->setValidator(new QDoubleValidator(led_scale3d_z));
    led_scale3d_z->setRange(0.001, 10.0);
    led_scale3d_z->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    led_scale3d_z_step = new DoubleEdit(DEF_PREC_SCALE, 0.1, DEF_DRAW3D_SCALE_Z_STEP * 0.1);
    led_scale3d_z_step->setValidator(new QDoubleValidator(led_scale3d_z_step));
    led_scale3d_z_step->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);

    pub_scale3d_reset = new QPushButton("reset");
    pub_scale3d_reset->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);


    /* ---------------------------------
    *    3-D  stereo
    * --------------------------------- */
    chb_stereo  = new QCheckBox("Use glasses");
    cob_glasses = new QComboBox();
    cob_glasses->addItems(stl_stereo_glasses);
    cob_glasses->setCurrentIndex((int)mParams->opengl_stereo_glasses);
    cob_glasses->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);

    cob_stereo_type = new QComboBox();
    cob_stereo_type->addItems(stl_stereo_types);
    cob_stereo_type->setCurrentIndex((int)mParams->opengl_stereo_type);
    cob_stereo_type->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);

    lab_eye_sep = new QLabel("eye sep");
    lab_eye_sep_value = new QLabel("value");
    lab_eye_sep_step  = new QLabel("step");
    led_eye_sep = new DoubleEdit(DEF_PREC_STEREO, mParams->opengl_stereo_sep, mParams->opengl_stereo_step);
    led_eye_sep->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    led_eye_sep_step = new DoubleEdit(DEF_PREC_STEREO, mParams->opengl_stereo_step, mParams->opengl_stereo_step * 0.1);
    led_eye_sep_step->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);

    if (mParams->opengl_stereo_use == 1) {
        chb_stereo->setChecked(true);
        led_eye_sep->setEnabled(true);
        led_eye_sep_step->setEnabled(true);
        cob_glasses->setEnabled(true);
        cob_stereo_type->setEnabled(true);
    } else {
        chb_stereo->setChecked(false);
        led_eye_sep->setEnabled(false);
        led_eye_sep_step->setEnabled(false);
        cob_glasses->setEnabled(false);
        cob_stereo_type->setEnabled(false);
    }

    chb_fog = new QCheckBox("fog");
    lab_fog_density = new QLabel("density");
    lab_fog_value   = new QLabel("value");
    lab_fog_step    = new QLabel("step");
    lab_fog_density->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_fog_density = new DoubleEdit(DEF_PREC_STEREO, mParams->opengl_fog_init, mParams->opengl_fog_step);
    led_fog_density->setRange(0.0, 1.0);
    led_fog_density->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    led_fog_density_step = new DoubleEdit(DEF_PREC_STEREO, mParams->opengl_fog_step, mParams->opengl_fog_step * 0.1);
    led_fog_density_step->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);

    if (mParams->opengl_fog_use == 1) {
        chb_fog->setChecked(true);
        led_fog_density->setEnabled(true);
        led_fog_density_step->setEnabled(true);
    } else {
        chb_fog->setChecked(false);
        led_fog_density->setEnabled(false);
        led_fog_density_step->setEnabled(false);
    }

    /* ---------------------------------
    *    3-D  animate
    * --------------------------------- */
    lab_anim_value = new QLabel("value");
    lab_anim_step = new QLabel("step");
    chb_anim_localrot = new QCheckBox("Local");
    chb_anim_localrot->setChecked(false);
    lab_anim_rotate_x = new QLabel("x-axis");
    led_anim_rotate_x = new DoubleEdit(DEF_PREC_ANIM, DEF_OPENGL_ANIM_ROT_X_INIT, 0.01);
    led_anim_rotate_x->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    led_anim_rotate_x_step = new DoubleEdit(DEF_PREC_ANIM, 0.01, 0.001);
    led_anim_rotate_x_step->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    lab_anim_rotate_y = new QLabel("y-axis");
    led_anim_rotate_y = new DoubleEdit(DEF_PREC_ANIM, DEF_OPENGL_ANIM_ROT_Y_INIT, 0.01);
    led_anim_rotate_y->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    led_anim_rotate_y_step = new DoubleEdit(DEF_PREC_ANIM, 0.01, 0.001);
    led_anim_rotate_y_step->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    lab_anim_rotate_z = new QLabel("z-axis");
    led_anim_rotate_z = new DoubleEdit(DEF_PREC_ANIM, DEF_OPENGL_ANIM_ROT_Z_INIT, 0.01);
    led_anim_rotate_z->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    led_anim_rotate_z_step = new DoubleEdit(DEF_PREC_ANIM, 0.01, 0.001);
    led_anim_rotate_z_step->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    pub_anim_rotate = new QPushButton("Play");
    pub_anim_rotate->setCheckable(true);
    pub_anim_rotate->setMaximumWidth(40);

    sli_anim_geodlength = new QSlider(Qt::Horizontal);
    sli_anim_geodlength->setRange(0, 0);
    lcd_anim_geodlength = new QLCDNumber();
    lcd_anim_geodlength->setSegmentStyle(QLCDNumber::Flat);

    QChar chAffine = mGreekLetter.toChar("lambda");
    lab_lastpoint_affineparam = new QLabel(chAffine);
    lab_lastpoint_affineparam->setMaximumSize(12, 20);
    led_lastpoint_affineparam = new QLineEdit();
    led_lastpoint_affineparam->setReadOnly(true);
    led_lastpoint_affineparam->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_lastpoint_affineparam->setAlignment(Qt::AlignRight);

    QStringList coordnames = QStringList() << "t" << "x" << "y" << "z";
    for (int i = 0; i < 4; i++) {
        QLabel* lab = new QLabel(coordnames[i]);
        lab->setMaximumSize(12, 20);
        lab->setMinimumSize(12, 20);
        lst_lab_lastpoint_coordname.push_back(lab);

        QLineEdit* led_pos = new QLineEdit();
        led_pos->setReadOnly(true);
        led_pos->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
        led_pos->setAlignment(Qt::AlignRight);
        lst_led_lastpoint_pos_value.push_back(led_pos);
    }

    tim_anim_rotate = new QTimer();
    tim_anim_rotate->setInterval(33);


    /* ---------------------------------
    *    3-D  embedding
    * --------------------------------- */
    tbw_emb_params = new QTableWidget(0, 3);
    QStringList tbw_emb_headers = QStringList() << "Name" << "Value" << "Step";
    tbw_emb_params->setHorizontalHeaderLabels(tbw_emb_headers);
    tbw_emb_params->setAlternatingRowColors(true);

    lab_emb_color = new QLabel("Color");
    lab_emb_color->setMaximumWidth(50);
    pub_emb_color = new QPushButton();
    pub_emb_color->setPalette(QPalette(mParams->opengl_emb_color));
    pub_emb_color->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    /* ---------------------------------
    *    mouse handle
    * --------------------------------- */
    lab_mousehandle = new QLabel("mouse");
    cob_mousehandle = new QComboBox();
    cob_mousehandle->addItems(stl_mouse_handle);
    cob_mousehandle->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
}

/*! Construct graphical user interface.
 */
void DrawView::initGUI() {
    /* ---------------------------------
    *    2-D
    * --------------------------------- */
    QGroupBox* grb_2d_scaling = new QGroupBox("Scaling and position");
    QGridLayout* layout_2d_scaling = new QGridLayout();
    layout_2d_scaling -> addWidget(lab_min,     0, 1, Qt::AlignHCenter | Qt::AlignVCenter);
    layout_2d_scaling -> addWidget(lab_max,     0, 2, Qt::AlignHCenter | Qt::AlignVCenter);

    layout_2d_scaling -> addWidget(lab_draw2d_abs, 1, 0);
    layout_2d_scaling -> addWidget(led_draw2d_x_min, 1, 1);
    layout_2d_scaling -> addWidget(led_draw2d_x_max, 1, 2);

    layout_2d_scaling -> addWidget(lab_draw2d_ord, 2, 0);
    layout_2d_scaling -> addWidget(led_draw2d_y_min, 2, 1);
    layout_2d_scaling -> addWidget(led_draw2d_y_max, 2, 2);

    layout_2d_scaling -> addWidget(pub_draw2d_center, 3, 1);
    layout_2d_scaling -> addWidget(pub_draw2d_reset, 4, 1);
    layout_2d_scaling -> addWidget(pub_draw2d_zoom_in, 3, 2);
    layout_2d_scaling -> addWidget(pub_draw2d_zoom_out, 4, 2);

    layout_2d_scaling -> addWidget(lab_draw2d_pos,   5, 0);
    layout_2d_scaling -> addWidget(led_draw2d_currX, 5, 1);
    layout_2d_scaling -> addWidget(led_draw2d_currY, 5, 2);

    grb_2d_scaling->setLayout(layout_2d_scaling);


    QGroupBox* grb_2d_colors = new QGroupBox("Colors and line properties");
    QGridLayout* layout_2d_colors = new QGridLayout();
    layout_2d_colors->addWidget(lab_draw2d_bgcolor, 0, 0);
    layout_2d_colors->addWidget(pub_draw2d_bgcolor, 0, 1);
    layout_2d_colors->addWidget(lab_draw2d_linewidth, 0, 2);
    layout_2d_colors->addWidget(spb_draw2d_linewidth, 0, 3);
    layout_2d_colors->addWidget(lab_draw2d_gridcolor, 1, 0);
    layout_2d_colors->addWidget(pub_draw2d_gridcolor, 1, 1);
    layout_2d_colors->addWidget(lab_draw2d_fgcolor, 1, 2);
    layout_2d_colors->addWidget(pub_draw2d_fgcolor, 1, 3);
    grb_2d_colors->setLayout(layout_2d_colors);

    QHBoxLayout*  layout_2d_proj = new QHBoxLayout();
    layout_2d_proj->addWidget(cob_drawtype, 0);
    layout_2d_proj->addWidget(lab_abscissa, 1);
    layout_2d_proj->addWidget(cob_abscissa, 2);
    layout_2d_proj->addWidget(lab_ordinate, 3);
    layout_2d_proj->addWidget(cob_ordinate, 4);

    QVBoxLayout* layout_2d = new QVBoxLayout();
    layout_2d->addLayout(layout_2d_proj);
    layout_2d->addWidget(grb_2d_scaling);
    layout_2d->addWidget(grb_2d_colors);
    wgt_draw_2d->setLayout(layout_2d);


    /* ---------------------------------
    *    3-D
    * --------------------------------- */
    QGridLayout*  layout_3d = new QGridLayout();
    layout_3d -> addWidget(lab_mousehandle, 0, 0);
    layout_3d -> addWidget(cob_mousehandle, 0, 1);
    layout_3d -> addWidget(lab_projection, 0, 2);
    layout_3d -> addWidget(cob_projection, 0, 3);
    layout_3d -> addWidget(lab_drawtype3d, 1, 0);
    layout_3d -> addWidget(cob_drawtype3d, 1, 1);
    layout_3d -> addWidget(lab_view,       1, 2);
    layout_3d -> addWidget(cob_view,       1, 3);

    QGroupBox* grb_3d_col = new QGroupBox("Colors and line props");
    QGridLayout* layout_3d_col = new QGridLayout();
    layout_3d_col -> addWidget(lab_fgcolor,   0, 0);
    layout_3d_col -> addWidget(pub_fgcolor,   0, 1);
    layout_3d_col -> addWidget(lab_bgcolor,   1, 0);
    layout_3d_col -> addWidget(pub_bgcolor,   1, 1);
    layout_3d_col -> addWidget(lab_linewidth, 0, 2);
    layout_3d_col -> addWidget(spb_linewidth, 0, 3);
    layout_3d_col -> addWidget(chb_linesmooth, 0, 4);
    grb_3d_col->setLayout(layout_3d_col);


    QGroupBox* grb_3d_pos = new QGroupBox("Camera parameters");
    QGridLayout* layout_3d_pos = new QGridLayout();
    layout_3d_pos -> addWidget(lab_eye_x, 0, 0);
    layout_3d_pos -> addWidget(led_eye_x, 0, 1);
    layout_3d_pos -> addWidget(lab_poi_x, 0, 2);
    layout_3d_pos -> addWidget(led_poi_x, 0, 3);
    layout_3d_pos -> addWidget(lab_eye_y, 1, 0);
    layout_3d_pos -> addWidget(led_eye_y, 1, 1);
    layout_3d_pos -> addWidget(lab_poi_y, 1, 2);
    layout_3d_pos -> addWidget(led_poi_y, 1, 3);
    layout_3d_pos -> addWidget(lab_eye_z, 2, 0);
    layout_3d_pos -> addWidget(led_eye_z, 2, 1);
    layout_3d_pos -> addWidget(lab_poi_z, 2, 2);
    layout_3d_pos -> addWidget(led_poi_z, 2, 3);
    layout_3d_pos -> addWidget(lab_fov,   3, 0);
    layout_3d_pos -> addWidget(led_fov,   3, 1);
    layout_3d_pos -> addWidget(pub_reset_poi, 3, 3);
    grb_3d_pos->setLayout(layout_3d_pos);

    layout_3d -> addWidget(grb_3d_pos, 2, 0, 1, 4);
    layout_3d -> addWidget(grb_3d_col, 3, 0, 1, 4);
    wgt_draw_3d->setLayout(layout_3d);

    /* ---------------------------------
    *    3-D scale
    * --------------------------------- */
    QGroupBox*  grb_scale3d = new QGroupBox("Apply global scale factor");
    QGridLayout*  layout_3d_scale = new QGridLayout();
    layout_3d_scale -> addWidget(lab_scale3d_value,  0, 1, Qt::AlignHCenter | Qt::AlignVCenter);
    layout_3d_scale -> addWidget(lab_scale3d_step,   0, 2, Qt::AlignHCenter | Qt::AlignVCenter);
    layout_3d_scale -> addWidget(lab_scale3d_x,      1, 0);
    layout_3d_scale -> addWidget(led_scale3d_x,      1, 1);
    layout_3d_scale -> addWidget(led_scale3d_x_step, 1, 2);
    layout_3d_scale -> addWidget(lab_scale3d_y,      2, 0);
    layout_3d_scale -> addWidget(led_scale3d_y,      2, 1);
    layout_3d_scale -> addWidget(led_scale3d_y_step, 2, 2);
    layout_3d_scale -> addWidget(lab_scale3d_z,      3, 0);
    layout_3d_scale -> addWidget(led_scale3d_z,      3, 1);
    layout_3d_scale -> addWidget(led_scale3d_z_step, 3, 2);
    layout_3d_scale -> addWidget(pub_scale3d_reset,  4, 1, 1, 2);
    grb_scale3d->setLayout(layout_3d_scale);
    QVBoxLayout* layout_3d_compl = new QVBoxLayout();
    layout_3d_compl->addWidget(grb_scale3d);
    layout_3d_compl->addSpacing(100);
    wgt_draw_3dscale->setLayout(layout_3d_compl);

    /* ---------------------------------
    *    3-D stereo
    * --------------------------------- */
    QGroupBox* grb_anaglyph = new QGroupBox("Stereo - anaglyph");
    QGridLayout*  layout_3d_anaglyph = new QGridLayout();
    layout_3d_anaglyph -> addWidget(chb_stereo,  0, 0);
    layout_3d_anaglyph -> addWidget(cob_glasses, 0, 1);
    layout_3d_anaglyph -> addWidget(cob_stereo_type, 0, 2);
    layout_3d_anaglyph -> addWidget(lab_eye_sep_value, 1, 1, Qt::AlignHCenter | Qt::AlignVCenter);
    layout_3d_anaglyph -> addWidget(lab_eye_sep_step,  1, 2, Qt::AlignHCenter | Qt::AlignVCenter);
    layout_3d_anaglyph -> addWidget(lab_eye_sep, 2, 0);
    layout_3d_anaglyph -> addWidget(led_eye_sep, 2, 1);
    layout_3d_anaglyph -> addWidget(led_eye_sep_step, 2, 2);
    grb_anaglyph->setLayout(layout_3d_anaglyph);

    QGroupBox* grb_fog = new QGroupBox("Fog - artificial");
    QGridLayout* layout_3d_fog = new QGridLayout();
    layout_3d_fog -> addWidget(chb_fog,   0, 0);
    layout_3d_fog -> addWidget(lab_fog_value, 0, 1, Qt::AlignHCenter | Qt::AlignVCenter);
    layout_3d_fog -> addWidget(lab_fog_step,  0, 2, Qt::AlignHCenter | Qt::AlignVCenter);
    layout_3d_fog -> addWidget(lab_fog_density, 1, 0);
    layout_3d_fog -> addWidget(led_fog_density, 1, 1);
    layout_3d_fog -> addWidget(led_fog_density_step, 1, 2);
    grb_fog->setLayout(layout_3d_fog);

    QVBoxLayout* layout_3d_stereo = new QVBoxLayout();
    layout_3d_stereo -> addWidget(grb_anaglyph);
    layout_3d_stereo -> addWidget(grb_fog);
    layout_3d_stereo->addSpacing(100);
    wgt_draw_3dstereo->setLayout(layout_3d_stereo);


    /* ---------------------------------
    *    3-D animate
    * --------------------------------- */
    QGroupBox* grb_rotate_anim = new QGroupBox("Rotation");
    QGridLayout* layout_rotate_anim = new QGridLayout();
    layout_rotate_anim -> addWidget(chb_anim_localrot, 0, 0);
    layout_rotate_anim -> addWidget(lab_anim_value,    0, 1, Qt::AlignHCenter | Qt::AlignVCenter);
    layout_rotate_anim -> addWidget(lab_anim_step,     0, 2, Qt::AlignHCenter | Qt::AlignVCenter);
    layout_rotate_anim -> addWidget(lab_anim_rotate_x,      1, 0);
    layout_rotate_anim -> addWidget(led_anim_rotate_x,      1, 1);
    layout_rotate_anim -> addWidget(led_anim_rotate_x_step, 1, 2);
    layout_rotate_anim -> addWidget(lab_anim_rotate_y,      2, 0);
    layout_rotate_anim -> addWidget(led_anim_rotate_y,      2, 1);
    layout_rotate_anim -> addWidget(led_anim_rotate_y_step, 2, 2);
    layout_rotate_anim -> addWidget(lab_anim_rotate_z,      3, 0);
    layout_rotate_anim -> addWidget(led_anim_rotate_z,      3, 1);
    layout_rotate_anim -> addWidget(led_anim_rotate_z_step, 3, 2);
    layout_rotate_anim -> addWidget(pub_anim_rotate,   1, 3, 3, 1);
    grb_rotate_anim->setLayout(layout_rotate_anim);

    QGroupBox* grb_geod_length = new QGroupBox("Show # points");
    QVBoxLayout* layout_geod_length = new QVBoxLayout();
    QHBoxLayout* layout_geod_points = new QHBoxLayout();
    layout_geod_points -> addWidget(sli_anim_geodlength);
    layout_geod_points -> addWidget(lcd_anim_geodlength);
    QGridLayout* layout_geod_last = new QGridLayout();
    layout_geod_last -> addWidget(lab_lastpoint_affineparam,      0, 0);
    layout_geod_last -> addWidget(led_lastpoint_affineparam,      0, 1);
    layout_geod_last -> addWidget(lst_lab_lastpoint_coordname[0], 1, 0);
    layout_geod_last -> addWidget(lst_led_lastpoint_pos_value[0], 1, 1);
    layout_geod_last -> addWidget(lst_lab_lastpoint_coordname[1], 1, 2);
    layout_geod_last -> addWidget(lst_led_lastpoint_pos_value[1], 1, 3);
    layout_geod_last -> addWidget(lst_lab_lastpoint_coordname[2], 2, 0);
    layout_geod_last -> addWidget(lst_led_lastpoint_pos_value[2], 2, 1);
    layout_geod_last -> addWidget(lst_lab_lastpoint_coordname[3], 2, 2);
    layout_geod_last -> addWidget(lst_led_lastpoint_pos_value[3], 2, 3);
    layout_geod_length -> addLayout(layout_geod_points);
    layout_geod_length -> addLayout(layout_geod_last);
    grb_geod_length->setLayout(layout_geod_length);

    QVBoxLayout* layout_anim = new QVBoxLayout();
    layout_anim -> addWidget(grb_rotate_anim);
    layout_anim -> addWidget(grb_geod_length);
    wgt_draw_3danim->setLayout(layout_anim);


    /* ---------------------------------
    *    3-D embedding
    * --------------------------------- */
    QGroupBox* grb_emb_params = new QGroupBox("Embedding parameters");
    QGridLayout* layout_emb_params = new QGridLayout();
    layout_emb_params -> addWidget(tbw_emb_params, 0, 0);
    grb_emb_params->setLayout(layout_emb_params);

    QGridLayout* layout_emb = new QGridLayout();
    layout_emb -> addWidget(grb_emb_params, 0, 0, 1, 2);
    layout_emb -> addWidget(lab_emb_color, 1, 0);
    layout_emb -> addWidget(pub_emb_color, 1, 1);
    wgt_draw_3demb->setLayout(layout_emb);


    /* ---------------------------------
    *    set all tab widgets
    * --------------------------------- */
    tab_draw->addTab(wgt_draw_3d,       "3D");
    tab_draw->addTab(wgt_draw_3dscale,  "3D scale");
    tab_draw->addTab(wgt_draw_3dstereo, "3D stereo");
    tab_draw->addTab(wgt_draw_3danim,   "3D anim");
    tab_draw->addTab(wgt_draw_3demb,    "3D emb");
    tab_draw->addTab(wgt_draw_2d,       "2D");


    QGroupBox*  grb_draw = new QGroupBox("DrawHandling");
    grb_draw->setMaximumWidth(DEF_GROUPBOX_WIDTH);
    QGridLayout* layout_draw = new QGridLayout();
    layout_draw -> addWidget(tab_draw);
    grb_draw->setLayout(layout_draw);

    QVBoxLayout* layout_complete = new QVBoxLayout();
    layout_complete->addWidget(grb_draw);
    setLayout(layout_complete);
}

/*! Connect signals and slots.
 */
void DrawView::initControl() {
    connect(cob_drawtype, SIGNAL(activated(int)), this, SLOT(slot_setDrawType()));
    connect(cob_drawtype3d, SIGNAL(activated(int)), this, SLOT(slot_setDrawType3d()));
    connect(cob_abscissa, SIGNAL(activated(int)), this, SLOT(slot_setAbsOrd()));
    connect(cob_ordinate, SIGNAL(activated(int)), this, SLOT(slot_setAbsOrd()));

    connect(led_draw2d_x_min, SIGNAL(editingFinished()), this, SLOT(slot_setScaling()));
    connect(led_draw2d_x_max, SIGNAL(editingFinished()), this, SLOT(slot_setScaling()));
    connect(led_draw2d_y_min, SIGNAL(editingFinished()), this, SLOT(slot_setScaling()));
    connect(led_draw2d_y_max, SIGNAL(editingFinished()), this, SLOT(slot_setScaling()));
    connect(pub_draw2d_reset, SIGNAL(pressed()), this, SLOT(slot_resetScaling()));
    connect(pub_draw2d_center, SIGNAL(pressed()), this, SLOT(slot_centerScaling()));
    connect(pub_draw2d_zoom_in,  SIGNAL(pressed()), this, SLOT(slot_zoom()));
    connect(pub_draw2d_zoom_out, SIGNAL(pressed()), this, SLOT(slot_zoom()));

    connect(pub_draw2d_fgcolor, SIGNAL(pressed()), this, SLOT(slot_set2dFGcolor()));
    connect(pub_draw2d_bgcolor, SIGNAL(pressed()), this, SLOT(slot_set2dBGcolor()));
    connect(pub_draw2d_gridcolor, SIGNAL(pressed()), this, SLOT(slot_set2dGridColor()));
    connect(spb_draw2d_linewidth, SIGNAL(valueChanged(int)), SLOT(slot_set2dLineWidth()));

    connect(cob_mousehandle, SIGNAL(activated(int)), this, SLOT(slot_setMouseHandle()));
    connect(cob_projection,  SIGNAL(activated(int)), this, SLOT(slot_setProjection()));
    connect(cob_view,        SIGNAL(activated(int)), this, SLOT(slot_setCameraPredefs()));

    connect(led_fov, SIGNAL(editingFinished()), this, SLOT(slot_setFieldOfView()));

    connect(pub_bgcolor,   SIGNAL(pressed()), this, SLOT(slot_setBGcolor()));
    connect(pub_fgcolor,   SIGNAL(pressed()), this, SLOT(slot_setFGcolor()));
    connect(pub_emb_color, SIGNAL(pressed()), this, SLOT(slot_setEmbColor()));
    connect(spb_linewidth, SIGNAL(valueChanged(int)), this, SLOT(slot_setLineWidth()));
    connect(chb_linesmooth, SIGNAL(clicked()), this, SLOT(slot_setSmoothLine()));

    connect(led_scale3d_x, SIGNAL(editingFinished()), this, SLOT(slot_set3dScaling()));
    connect(led_scale3d_y, SIGNAL(editingFinished()), this, SLOT(slot_set3dScaling()));
    connect(led_scale3d_z, SIGNAL(editingFinished()), this, SLOT(slot_set3dScaling()));
    connect(led_scale3d_x_step, SIGNAL(editingFinished()), this, SLOT(slot_set3dScalingStep()));
    connect(led_scale3d_y_step, SIGNAL(editingFinished()), this, SLOT(slot_set3dScalingStep()));
    connect(led_scale3d_z_step, SIGNAL(editingFinished()), this, SLOT(slot_set3dScalingStep()));
    connect(pub_scale3d_reset, SIGNAL(pressed()), this, SLOT(slot_set3dScalingReset()));

    connect(led_eye_x, SIGNAL(editingFinished()), this, SLOT(slot_setCameraPos()));
    connect(led_eye_y, SIGNAL(editingFinished()), this, SLOT(slot_setCameraPos()));
    connect(led_eye_z, SIGNAL(editingFinished()), this, SLOT(slot_setCameraPos()));
    connect(led_poi_x, SIGNAL(editingFinished()), this, SLOT(slot_setCameraPoi()));
    connect(led_poi_y, SIGNAL(editingFinished()), this, SLOT(slot_setCameraPoi()));
    connect(led_poi_z, SIGNAL(editingFinished()), this, SLOT(slot_setCameraPoi()));
    connect(pub_reset_poi, SIGNAL(pressed()),   this, SLOT(slot_resetPoi()));

    connect(chb_stereo,  SIGNAL(stateChanged(int)), this, SLOT(slot_setStereo()));
    connect(cob_glasses, SIGNAL(activated(int)),    this, SLOT(slot_setStereoParams()));
    connect(cob_stereo_type, SIGNAL(activated(int)),    this, SLOT(slot_setStereoType()));
    connect(led_eye_sep_step, SIGNAL(editingFinished()), this, SLOT(slot_setStereoStep()));
    connect(led_eye_sep,      SIGNAL(editingFinished()), this, SLOT(slot_setStereoParams()));

    connect(chb_fog,              SIGNAL(stateChanged(int)), this, SLOT(slot_setFog()));
    connect(led_fog_density,      SIGNAL(editingFinished()),   this, SLOT(slot_setFogDensity()));
    connect(led_fog_density_step, SIGNAL(editingFinished()),   this, SLOT(slot_setFogDensityStep()));

    connect(mDraw,   SIGNAL(scalingChanged()), this, SLOT(slot_adjustScaling()));
    connect(mDraw,   SIGNAL(scalingReset()), this, SLOT(slot_resetScaling()));
    connect(mOpenGL, SIGNAL(cameraMoved()),    this, SLOT(slot_adjustCamera()));

    connect(led_anim_rotate_x, SIGNAL(editingFinished()), this, SLOT(slot_setAnimRotateParams()));
    connect(led_anim_rotate_y, SIGNAL(editingFinished()), this, SLOT(slot_setAnimRotateParams()));
    connect(led_anim_rotate_z, SIGNAL(editingFinished()), this, SLOT(slot_setAnimRotateParams()));
    connect(led_anim_rotate_x_step, SIGNAL(editingFinished()), this, SLOT(slot_setAnimRotateParamsStep()));
    connect(led_anim_rotate_y_step, SIGNAL(editingFinished()), this, SLOT(slot_setAnimRotateParamsStep()));
    connect(led_anim_rotate_z_step, SIGNAL(editingFinished()), this, SLOT(slot_setAnimRotateParamsStep()));

    connect(pub_anim_rotate, SIGNAL(toggled(bool)), this, SLOT(slot_anim_rot_startstop()));
    connect(tim_anim_rotate, SIGNAL(timeout()), this, SLOT(slot_anim_rotate()));

    connect(sli_anim_geodlength, SIGNAL(valueChanged(int)), lcd_anim_geodlength, SLOT(display(int)));
    connect(sli_anim_geodlength, SIGNAL(valueChanged(int)), this, SLOT(slot_showNumPoints(int)));
}

/*! Initialize status tips.
 */
void DrawView::initStatusTips() {
#ifdef SHOW_STATUS_TIPS
    cob_drawtype  -> setStatusTip(tr("Set draw type for 2D view."));
    cob_abscissa  -> setStatusTip(tr("Select coordinate for abscissa."));
    cob_ordinate  -> setStatusTip(tr("Select coordinate for ordinate."));

    led_draw2d_x_min -> setStatusTip(tr("Set minimum of 2D view for abscissa."));
    led_draw2d_x_max -> setStatusTip(tr("Set maximum of 2D view for abscissa."));
    led_draw2d_y_min -> setStatusTip(tr("Set minimum of 2D view for ordinate."));
    led_draw2d_y_max -> setStatusTip(tr("Set maximum of 2D view for ordinate."));
    led_draw2d_currX -> setStatusTip(tr("Current cursor position (abscissa)."));
    led_draw2d_currY -> setStatusTip(tr("Current cursor position (ordinate)."));

    cob_mousehandle  -> setStatusTip(tr("Select mouse control."));
    cob_projection   -> setStatusTip(tr("Projection type for 3D view."));
    cob_drawtype3d   -> setStatusTip(tr("Representation for 3D view."));
    cob_view         -> setStatusTip(tr("Select predefined relative camera position."));
    led_eye_x        -> setStatusTip(tr("Camera's x position."));
    led_eye_y        -> setStatusTip(tr("Camera's y position."));
    led_eye_z        -> setStatusTip(tr("Camera's z position."));
    led_poi_x        -> setStatusTip(tr("Camera's point of interest."));
    led_poi_y        -> setStatusTip(tr("Camera's point of interest."));
    led_poi_z        -> setStatusTip(tr("Camera's point of interest."));

    led_scale3d_x      -> setStatusTip(tr("Scale factor for 3D view."));
    led_scale3d_x_step -> setStatusTip(tr("Step size for scale factor."));
    led_scale3d_y      -> setStatusTip(tr("Scale factor for 3D view."));
    led_scale3d_y_step -> setStatusTip(tr("Step size for scale factor."));
    led_scale3d_z      -> setStatusTip(tr("Scale factor for 3D view."));
    led_scale3d_z_step -> setStatusTip(tr("Step size for scale factor."));

    chb_stereo       -> setStatusTip(tr("Use anaglyph for stereo."));
    cob_glasses      -> setStatusTip(tr("Select anaglyph glasses type."));
    cob_stereo_type  -> setStatusTip(tr("Set type of stereo projection."));
    led_eye_sep      -> setStatusTip(tr("Camera eye separation."));
    led_eye_sep_step -> setStatusTip(tr("Step size for eye separation."));
    chb_fog          -> setStatusTip(tr("Use artificial fog."));
    led_fog_density  -> setStatusTip(tr("Density of fog."));
    led_fog_density_step -> setStatusTip(tr("Step size for density."));

    chb_anim_localrot -> setStatusTip(tr("Rotate around local axes."));
    led_anim_rotate_x -> setStatusTip(tr("Velocity of rotation around x axis."));
    led_anim_rotate_x_step -> setStatusTip(tr("Step size for rotation around x axis."));
    led_anim_rotate_y -> setStatusTip(tr("Velocity of rotation around y axis."));
    led_anim_rotate_y_step -> setStatusTip(tr("Step size for rotation around y axis."));
    led_anim_rotate_z -> setStatusTip(tr("Velocity of rotation around z axis."));
    led_anim_rotate_z_step -> setStatusTip(tr("Step size for rotation around z axis."));
    sli_anim_geodlength -> setStatusTip(tr("Number of points to be shown."));
#endif
}
