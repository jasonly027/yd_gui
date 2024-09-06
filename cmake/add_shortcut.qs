function Component() {}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/bin/yd_gui.exe", "@StartMenuDir@/yd_gui.lnk");
    }
}
