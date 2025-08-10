#ifndef MATERIALEDIT_H
#define MATERIALEDIT_H

#include <QWidget>
#include <Windows/QT/GlassMaterialEdit.h>
#include <Windows/QT/ConductorMaterialEdit.h>
#include <Windows/QT/DiffuseMaterialEdit.h>
#include <Scene/SceneData.h>
#include <Scene/SceneWrapper.h>
namespace Ui {
class MaterialEdit;
}

class MaterialEdit : public QWidget
{
    Q_OBJECT
public:
    explicit MaterialEdit(QWidget *parent = 0);
    ~MaterialEdit();
    void SetSceneWrapper(Render::SceneWrapper* wrapper);
    void SetSceneData(Render::SceneData* p);
    void SetMaterialName(std::string name);
signals:
    void TextureChanged();
private:
    Ui::MaterialEdit *ui;
    GlassMaterialEdit* GME = nullptr;
    DiffuseMaterialEdit* DME = nullptr;
    ConductorMaterialEdit* CME = nullptr;
    int MaterialIndex = -1;
    std::string MaterialType;   // glass, diffuse, conductor
    std::string MaterialName;
    int TextureIndex = -1;
    int TextureType = -1; // 0: image, 1: const, 2: noise 3: random
    std::string TextureName;
    Render::SceneData* SceneData = nullptr;
    Render::SceneWrapper* SceneWrapper = nullptr;
};

#endif // MATERIALEDIT_H
