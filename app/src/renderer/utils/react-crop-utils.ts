import type { Area } from "react-easy-crop";

const OUTPUT_WIDTH = 320;
const OUTPUT_HEIGHT = 240;

export async function getCroppedImg(
  imageSrc: string,
  pixelCrop: Area,
  rotation = 0,
  flip = { horizontal: false, vertical: false },
) {
  const image = await createImage(imageSrc);

  const canvas = document.createElement("canvas");
  const ctx = canvas.getContext("2d");

  if (!ctx) {
    return null;
  }

  const rotRad = getRadianAngle(rotation);

  const { width: bBoxWidth, height: bBoxHeight } = rotateSize(
    image.width,
    image.height,
    rotation,
  );

  canvas.width = bBoxWidth;
  canvas.height = bBoxHeight;

  ctx.translate(bBoxWidth / 2, bBoxHeight / 2);
  ctx.rotate(rotRad);
  ctx.scale(flip.horizontal ? -1 : 1, flip.vertical ? -1 : 1);
  ctx.translate(-image.width / 2, -image.height / 2);

  ctx.drawImage(image, 0, 0);

  // Final output is ALWAYS 320 × 240
  const croppedCanvas = document.createElement("canvas");
  const croppedCtx = croppedCanvas.getContext("2d");

  if (!croppedCtx) {
    return null;
  }

  croppedCanvas.width = OUTPUT_WIDTH;
  croppedCanvas.height = OUTPUT_HEIGHT;

  croppedCtx.drawImage(
    canvas,
    pixelCrop.x,
    pixelCrop.y,
    pixelCrop.width,
    pixelCrop.height,
    0,
    0,
    OUTPUT_WIDTH,
    OUTPUT_HEIGHT,
  );

  return new Promise<string | null>((resolve) => {
    croppedCanvas.toBlob(
      (file) => {
        resolve(file ? URL.createObjectURL(file) : null);
      },
      "image/jpeg",
      0.92,
    );
  });
}

function getRadianAngle(degreeValue: number) {
  return (degreeValue * Math.PI) / 180;
}

function rotateSize(width: number, height: number, rotation: number) {
  const rotRad = getRadianAngle(rotation);

  return {
    width:
      Math.abs(Math.cos(rotRad) * width) + Math.abs(Math.sin(rotRad) * height),
    height:
      Math.abs(Math.sin(rotRad) * width) + Math.abs(Math.cos(rotRad) * height),
  };
}

export async function imageToRGB565Base64(imageSrc: string): Promise<string> {
  const image = await createImage(imageSrc);

  const canvas = document.createElement("canvas");
  canvas.width = OUTPUT_WIDTH;
  canvas.height = OUTPUT_HEIGHT;

  const ctx = canvas.getContext("2d", {
    willReadFrequently: true,
  });

  if (!ctx) {
    throw new Error("Could not get canvas context");
  }

  ctx.drawImage(image, 0, 0, OUTPUT_WIDTH, OUTPUT_HEIGHT);

  const { data } = ctx.getImageData(0, 0, OUTPUT_WIDTH, OUTPUT_HEIGHT);

  // 2 bytes per pixel
  const rgb565 = new Uint8Array(OUTPUT_WIDTH * OUTPUT_HEIGHT * 2);

  let offset = 0;

  for (let i = 0; i < data.length; i += 4) {
    const r = data[i];
    const g = data[i + 1];
    const b = data[i + 2];

    // RGB888 -> RGB565
    const value = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

    // Big-endian: high byte first
    rgb565[offset++] = (value >> 8) & 0xff;
    rgb565[offset++] = value & 0xff;
  }

  // Uint8Array -> binary string -> Base64
  let binary = "";

  const CHUNK_SIZE = 0x8000;

  for (let i = 0; i < rgb565.length; i += CHUNK_SIZE) {
    binary += String.fromCharCode(...rgb565.subarray(i, i + CHUNK_SIZE));
  }

  return btoa(binary);
}

function createImage(url: string) {
  return new Promise<HTMLImageElement>((resolve, reject) => {
    const image = new Image();

    image.addEventListener("load", () => resolve(image));
    image.addEventListener("error", reject);

    image.setAttribute("crossOrigin", "anonymous");
    image.src = url;
  });
}
