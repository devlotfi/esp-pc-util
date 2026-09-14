import { build, Platform, type Configuration } from "electron-builder";

const options = {
  // "store” | “normal” | "maximum". - For testing builds, using 'store' can significantly reduce build time.
  compression: "normal",
  removePackageScripts: true,

  directories: {
    output: "release/windows",
    buildResources: "res",
  },
  files: ["dist/**/*"],

  win: {
    target: "nsis",
  },
} satisfies Configuration;

// Promise is returned
const result = await build({
  targets: Platform.WINDOWS.createTarget(),
  config: options,
});
console.log(JSON.stringify(result));
