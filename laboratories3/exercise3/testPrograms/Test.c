#define LOOPS 1E9

int main() {
  int a, b;

  int i;
  for(i = 0; i < LOOPS; i++) {
    a = i;
    b = a;
  }
  return 0;
}
