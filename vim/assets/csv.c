#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char    *sub, *line;
  double  d;
  int     i, j, r, c;
  FILE    *f;
  
  if (argc < 2)
  {
    printf("Usage: 080100718 filename\n");
    return 1;
  }

  f = fopen(argv[1], "r");
  if (!f)
  {
    printf("Cannot open '%s'\n", argv[1]);
    return 1;
  }

  line = malloc(1024*1024);
  sub  = malloc(1024);

  if (!line || !sub) return 1;


  /* first line */
  fscanf(f, "%d,%d", &r, &c);
  printf("r=%d c=%d\n", r, c);

  /* other lines */
  while (r--)
  {
    memset(line, 0, 1024 * 1024);
    memset(sub,  0, 1024);
    fscanf(f, "%s", line);
    printf("I read line '%s'\n", line);
    i = j = 0;
    while (line[i])
    {
      if (0) printf("char '%c' %02x\n", line[i], line[i]);
      /* interpret buffer */
      if (line[i] == ',' || (i == (strlen(line) - 1)))
      {
        d = atof(sub);
        printf("double %lf\n", d);
        memset(sub, 0, 1024);
        j = 0;
      }
      else
        sub[j++] = line[i];
      
      i++;
    }
  }
  fclose(f);

  free(line);
  free(sub);

  return 0;
}
