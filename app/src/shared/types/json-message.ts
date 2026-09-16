interface SetLedMessage {
  type: "SET_LED";
  color: string;
  brightness: number;
}

interface SetWallpaperMessage {
  type: "SET_WALLPAPER";
  image: string;
}

export type JsonMessage = SetLedMessage | SetWallpaperMessage;
