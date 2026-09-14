export const sameElements = (a: string[], b: string[]): boolean => {
  if (a.length !== b.length) return false;

  // Define the map with string keys and number values
  const map: Record<string, number> = {};

  for (const str of a) {
    map[str] = (map[str] || 0) + 1;
  }

  for (const str of b) {
    // If the element doesn't exist or count is already 0
    if (!map[str]) {
      return false;
    }
    map[str]--;
  }

  return true;
};
