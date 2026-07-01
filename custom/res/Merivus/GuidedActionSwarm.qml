import QGroundControl.FlightDisplay 1.0

GuidedToolStripAction {
    text:       _guidedController.swarmTitle
    iconSource: "/qmlimages/swarm.svg"
    visible:    true
    enabled:    _guidedController.showTakeoff
    actionID:   _guidedController.actionSwarm
}
