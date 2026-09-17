interface SetLedMessage {
  type: "SET_LED";
  color: string;
  brightness: number;
}

interface SetDisplayMessage {
  type: "SET_DISPLAY";
  accentColor: string;
  brightness: number;
}

interface SetWallpaperMessage {
  type: "SET_WALLPAPER";
  image: string;
}

interface SetWallpaperCompletedMessage {
  type: "SET_WALLPAPER_COMPLETED";
}

interface PcStatsMessage {
  type: "PC_STATS";
  cpu: number;
  ram: number;
}

interface GetDataMessage {
  type: "GET_DATA";
}

interface GetDataResponseMessage {
  type: "GET_DATA_RESPONSE";
  led: {
    color: string;
    brightness: number;
  };
  display: {
    accentColor: string;
    brightness: number;
  };
}

export type JsonMessage =
  | SetLedMessage
  | SetDisplayMessage
  | SetWallpaperMessage
  | SetWallpaperCompletedMessage
  | PcStatsMessage
  | GetDataMessage
  | GetDataResponseMessage;
