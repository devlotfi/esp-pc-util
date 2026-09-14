import { defineConfig } from "vite";

// https://vite.dev/config/
export default defineConfig({
  publicDir: false,
  build: {
    emptyOutDir: true,
    outDir: "dist/main",
    ssr: true,
    target: "node22",
    rolldownOptions: {
      output: {
        format: "esm",
      },
      input: "src/main/main.ts",
    },
  },
});
