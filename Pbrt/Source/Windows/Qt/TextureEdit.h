#ifndef TEXTUREEDIT_H
#define TEXTUREEDIT_H

#include <QWidget>

namespace Ui {
class TextureEdit;
}

class TextureEdit : public QWidget
{
    Q_OBJECT

public:
    explicit TextureEdit(QWidget *parent = 0);
    ~TextureEdit();
   
    void SetTextureName(QString name);
    void SetTextureType(int type);
    void SetTextureIndex(int index);
    void SetTextureImage(const std::string &filename);
    void SetTextureColor(float r, float g, float b);

signals:
    void TextureChanged();
public slots:
    void colorEdit();
    void fileEdit();
private:
    Ui::TextureEdit *ui;
    int index = -1;
};

#endif // TEXTUREEDIT_H
