import QtQuick 2.12

// MERIVUS moves the flight actions into CommandCenterOverlay.
// Keep the interface expected by FlyViewWidgetLayer without reserving map space.
Item {
    signal displayPreFlightChecklist
    property real maxHeight: 0
    width: 0
    height: 0
}
