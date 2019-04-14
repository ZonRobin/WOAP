const inputName = "build/index.js";
const outputName = "build/WOAP WebProxy.exe";
const outputTarget = "win-x86";
const customIconPath = "icon.ico";
var pkgPrecompiledBinaries = null;

const fs = require("fs");
const resourceHacker = require("node-resourcehacker");
const execSync = require("child_process").execSync;

downloadPkgPrecompiledBinaries((downloaded) => {
    if (downloaded) {
        customizePkgPrecompiledBinaries(() => {
            buildCustomizedExecutables();
            console.log("Done");
        });
    } else {
        buildCustomizedExecutables();
        console.log("Done");
    }
});

function downloadPkgPrecompiledBinaries(callback) {
    pkgPrecompiledBinaries = getPkgPrecompiledBinaryName();

    if (!fs.existsSync(pkgPrecompiledBinaries)) {
        console.log("Downloading pkg precompiled libraries");
        executePkg("build/temp.exe");
        fs.unlinkSync("build/temp.exe");

        pkgPrecompiledBinaries = getPkgPrecompiledBinaryName();

        if (pkgPrecompiledBinaries === null) {
            console.error("Could not found pkg libraries");
            return;
        }

        callback(true);
        return;
    }

    if (pkgPrecompiledBinaries === null) {
        console.error("Could not found pkg libraries");
        return;
    }
    callback(false);
}

function buildCustomizedExecutables() {
    console.log("Building customized executables");
    executePkg("build/output.exe");
    fs.renameSync("build/output.exe", outputName);
}

function customizePkgPrecompiledBinaries(callback) {
    console.log("Customizing pkg precompiled libraries");

    modifyResource({
        operation: "delete",
        input: pkgPrecompiledBinaries,
        output: pkgPrecompiledBinaries,
        resource: customIconPath,
        resourceType: "ICON,,",
        resourceName: "1"
    }).then(() => {
        return modifyResource({
            operation: "delete",
            input: pkgPrecompiledBinaries,
            output: pkgPrecompiledBinaries,
            resource: customIconPath,
            resourceType: "VERSIONINFO",
            resourceName: "1"
        });
    }, (err) => {
        console.error("Could not modify pkg binaries");
        console.error(err);
    }).then(() => {
        return modifyResource({
            operation: "addoverwrite",
            input: pkgPrecompiledBinaries,
            output: pkgPrecompiledBinaries,
            resource: customIconPath,
            resourceType: "ICONGROUP,MAINICON",
            resourceName: "1"
        });
    }, (err) => {
        console.error("Could not modify pkg binaries");
        console.error(err);
    }).then(() => {
        callback();
    }, (err) => {
        console.error("Could not modify pkg binaries");
        console.error(err);
    });
}

function modifyResource(options) {
    return new Promise((resolve, reject) => {
        resourceHacker(options, (err) => {
            if (err === null)
                resolve();
            else
                reject(err);
        });
    });
}

function executePkg(exeName) {
    execSync(
        "node_modules\\.bin\\yarn run cross-env PKG_CACHE_PATH=./build/assets pkg " + inputName +
        " --target " + outputTarget +
        " --output " + exeName
    );
}

function getPkgPrecompiledBinaryName() {
    const assetsRoot = "./build/assets";

    if (!fs.existsSync(assetsRoot))
        return null;

    const filesRoot = fs.readdirSync(assetsRoot);

    if (filesRoot.length === 0)
        return null;

    const assetDir = assetsRoot + "/" + filesRoot[0];
    if (!fs.lstatSync(assetDir).isDirectory())
        return null;

    const files = fs.readdirSync(assetDir);

    if (files.length === 1)
        return assetDir + "/" + files[0];

    return null;
}