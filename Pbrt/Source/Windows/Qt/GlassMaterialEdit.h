#ifndef GLASSMATERIALEDIT_H
#define GLASSMATERIALEDIT_H

#include <QWidget>
#include <Scene/SceneData.h>
#include <Scene/SceneWrapper.h>
namespace Ui {
class GlassMaterialEdit;
}

class GlassMaterialEdit : public QWidget
{
    Q_OBJECT
public:
    explicit GlassMaterialEdit(QWidget *parent = 0);
    ~GlassMaterialEdit();
    void SetSceneWrapper(Render::SceneWrapper* wrapper);
    void SetSceneData(Render::SceneData* p);
    void SetMaterialName(std::string name);
signals:
    void TextureChanged();
public slots:
    void TextureEdit();
    void TransmittanceChanged();
    void ReflectivityChanged();
private:
    void TextureColorEdit();
    void TextureImageEdit();
    void SetUIColor(QColor color); 
    void SetUITextureImageFileName(const std::string& filename);
    void SetTransmittance(float t);
    void SetReflectivity(float r); 
    void SetIOR(float ior);
private:
    Ui::GlassMaterialEdit *ui;
    int TextureIndex = -1;
    int TextureType = -1; // 0: image file, 1: rgb
    int MaterialIndex = -1;
    std::string MaterialName;
    Render::SceneData* SceneData = nullptr;
    Render::SceneWrapper* SceneWrapper = nullptr;
};

#endif // GLASSMATERIALEDIT_H
