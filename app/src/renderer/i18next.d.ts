import "i18next";
import { AppTranslation } from "./types/app-translation";

declare module "i18next" {
  interface CustomTypeOptions {
    resources: {
      translation: AppTranslation;
    };
  }
}

type a = {
  a: string;
};

type b = {
  b: string;
};

type c = a | b;

const lol: c = {
  a: "lol",
  b: "lol",
};
