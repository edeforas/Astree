#ifndef DOCKCOMMENTARY_H
#define DOCKCOMMENTARY_H

#include <QDockWidget>

class OpticalDevice;
namespace Ui {
class DockComment;
}

class DockCommentary : public QDockWidget {
    Q_OBJECT
public:
    DockCommentary(QWidget *parent = 0);
    ~DockCommentary();

    void device_changed(OpticalDevice* pDevice,int iReason);

signals:
    void device_changed(void* pSender);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DockComment *ui;
    OpticalDevice* _pDevice;
    bool _bBlockSignals;
private slots:
    void on_textEdit_textChanged();
};

#endif // DOCKCOMMENTARY_H
