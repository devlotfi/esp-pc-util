import { defineConfig } from "vite";

// https://vite.dev/config/
export default defineConfig({
  publicDir: false,
  build: {
    emptyOutDir: true,
    outDir: "dist/preload",
    ssr: true,
    target: "node22",
    rolldownOptions: {
      output: {
        format: "cjs",
      },
      input: "src/preload/preload.ts",
    },
  },
});
