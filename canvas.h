#ifndef CANVAS_H
#define CANVAS_H

//===-- qlogo/canvas.h - Canvas class definition -------*- C++ -*-===//
//
// This file is part of QLogo.
//
// QLogo is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// QLogo is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with QLogo.  If not, see <http://www.gnu.org/licenses/>.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Canvas class, which is the
/// graphics portion of the user interface.
///
//===----------------------------------------------------------------------===//

#include "turtle.h"
#include <QList>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QVector>

class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

struct Label {
  QString text;
  QVector4D position;
  QColor color;
  QFont font;

  Label(const QString &aText, const QVector4D &aPosition, const QColor &aColor,
        const QFont &aFont)
      : text(aText), position(aPosition), color(aColor), font(aFont) {}
};

enum CanvasDrawingEventType {
  canvasDrawArrayType,
  canvasDrawSetPenmodeType,
  canvasDrawSetPensizeType
};

struct CanvasDrawingArrayEvent {
  GLenum mode;
  GLint first;
  GLsizei count;
};

struct CanvasDrawingSetPenmodeEvent {
  PenModeEnum penMode;
};

struct CanvasDrawingSetPensizeEvent {
  GLfloat width;
};

union CanvasDrawingEventU {
  CanvasDrawingArrayEvent drawArrayEvent;
  CanvasDrawingSetPenmodeEvent penmodeEvent;
  CanvasDrawingSetPensizeEvent pensizeEvent;
};

struct CanvasDrawingEvent {
  CanvasDrawingEventType type;
  CanvasDrawingEventU u;
};

class Canvas : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

protected:
  QMatrix4x4 matrix;
  QMatrix4x4 invertedMatrix;
  int w;
  int h;

  QPointF worldToScreen(const QVector4D &world);
  QVector2D screenToWorld(const QPointF &p);

  qreal boundsX;
  qreal boundsY;

  QList<Label> labels;

  void initializeGL() override;
  void resizeGL(int width, int height) override;
  void paintGL() override;

  void paintSurface();
  void paintTurtle();

  GLuint matrixUniformID;
  void updateMatrix(void);

  QOpenGLShaderProgram *shaderProgram;

  // Border Surface VBO
  QOpenGLVertexArrayObject *surfaceArrayObject;
  QOpenGLBuffer *surfaceVertexBufferObject;
  QOpenGLBuffer *surfaceColorBufferObject;

  // LOGO Drawing VBO
  QOpenGLVertexArrayObject *linesObject = NULL;
  QOpenGLBuffer *linesVertexBufferObject;
  QOpenGLBuffer *linesColorBufferObject;

  // Turtle Drawing VBO
  QOpenGLVertexArrayObject *t_object = NULL;
  QOpenGLBuffer *t_vertex_bo;
  QOpenGLBuffer *t_color_bo;
  QOpenGLBuffer *t_index_bo;

  // User-generated lines
  QVector<GLfloat> vertices;
  QVector<GLubyte> vertexColors;

  PenModeEnum currentPenMode;

  GLfloat pensizeRange[2];
  GLfloat currentPensize = 0;

  QList<CanvasDrawingEvent> drawingEventList;
  GLclampf backgroundColor[4];

  void makeTurtleVBO(void);
  void makeSurfaceVBO(void);
  void setSurfaceVertices(void);
  void renderLabels(QPainter *painter);

  void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
  void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
  void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;

  bool isBounded = true;

public:
  Canvas(QWidget *parent = 0);

  void setBackgroundColor(const QColor &c);

  void addLine(const QVector4D &vertexA, const QVector4D &vertexB,
               const QColor &color);
  void addPolygon(const QList<QVector4D> &points, const QList<QColor> &colors);
  void addLabel(const QString &aText, const QVector4D &aLocation,
                const QColor &aColor, const QFont &aFont);

  void clearScreen();
  void setBounds(qreal x, qreal y);
  void getBounds(qreal &x, qreal &y);
  QImage getImage();

  bool getIsBounded();
  void setIsBounded(bool aIsBounded);

  void setPenmode(PenModeEnum newMode);

  void setPensize(GLfloat aSize);
  bool isPenSizeValid(GLfloat aSize);
};

#endif // CANVAS_H
