import QtQuick                    2.12
import QGroundControl.Palette     1.0

Item {
    id: root
    visible: false

    QGCPalette { id: palette; colorGroupEnabled: true }

    function colorFor(severity) {
        if (severity === "normal") return palette.colorGreen
        if (severity === "warning") return palette.colorOrange
        if (severity === "critical") return palette.colorRed
        return palette.colorGrey
    }
}
