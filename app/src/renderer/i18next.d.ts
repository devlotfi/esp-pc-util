import "i18next";
import type { EN } from "./translation/en";

declare module "i18next" {
  interface CustomTypeOptions {
    resources: {
      translation: typeof EN;
    };
  }
}
