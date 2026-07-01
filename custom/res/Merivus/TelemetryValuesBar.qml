import QtQuick 2.12

// MERIVUS presents the same core flight facts in CommandCenterOverlay.
// Keep this component as a zero-size compatibility item for FlyViewWidgetLayer.
Item {
    property bool bottomMode: true
    visible: false
    width: 0
    height: 0
}
