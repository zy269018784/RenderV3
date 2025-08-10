#ifndef DIFFUSEMATERIALEDIT_H
#define DIFFUSEMATERIALEDIT_H

#include <QWidget>
#include <Scene/SceneData.h>
namespace Ui {
class DiffuseMaterialEdit;
}

class DiffuseMaterialEdit : public QWidget
{
    Q_OBJECT
public:
    explicit DiffuseMaterialEdit(QWidget *parent = 0);
    ~DiffuseMaterialEdit();
    void SetSceneData(Render::SceneData* p);
    void SetMaterialName(std::string name);
signals:
    void TextureChanged();
public slots:
    void TextureEdit();
private:
    void TextureColorEdit();
    void TextureImageEdit();
    void SetUIColor(QColor color); 
    void SetUITextureImageFileName(const std::string& filename);
private:
    Ui::DiffuseMaterialEdit *ui;
    std::string TextureName;
    int TextureIndex = -1;
    int TextureType = -1; // 0: image file, 1: rgb
    std::string MaterialName;
    int MaterialIndex = -1;
    Render::SceneData* SceneData = nullptr;
};

#endif // DIFFUSEMATERIALEDIT_H
