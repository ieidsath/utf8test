/*

    utf8test: A simple UTF-8 decoder that prints readable Unicode code points

    Copyright © 2017 Ian Eidsath

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  FILE *fp;
  if(argc > 1) {
    fp = fopen(argv[1], "r");
  }
  else {
    fp = fopen("/dev/stdin", "r");
  }
  if(fp == NULL) {
    if(argc > 1) {
      fprintf(stderr, "Error opening %s, exiting...\n", argv[1]);
    }
    else {
      fprintf(stderr, "Error opening stdin, exiting...\n");
    }
    exit(1);
  }

  unsigned char *b;
  b = (unsigned char *)malloc(1024);
  if(b == NULL) {
    fprintf(stderr, "Could not allocate string space, exiting...\n");
    exit(1);
  }

  int i, bwidth, strbytes, uc;

  while(!feof(fp)) {

    fgets((char *)b,1024,fp);
    strbytes = strnlen(b,1024);

    i = 0;
    while( i < strbytes ) {

      bwidth = -1;
      uc = -1;

      if( ((int)b[i] & 0x80) == 0 ) { // Valid 1-byte UTF-8 character, U+00 - U+7F
        uc = (int)b[i];
        bwidth = 1;
      }
      else if( ((int)b[i] & 0xc0) == 0x80 ) { // Unexpected continue byte
        uc = -1;
      }
      else if( ( ((int)b[i] & 0xe0) == 0xc0 ) && i+1 < strbytes ) { // Start of 2-byte encoding
        if( ((int)b[i+1] & 0xc0) == 0x80 ) { // 2-byte headers
          uc = 64 * ( (int)b[i] & 0x1f ) + ( (int)b[i+1] & 0x3f );
          if( uc > 0x7f ) { // Valid 2-byte UTF-8 character U+80 - U+7FF

            bwidth = 2;
          }
          else { // Overlong 2-byte UTF-8 (non-standard)

            bwidth = 2;
          }
        }
        else { // Invalid continue byte
          uc = -1;
        }
      }
      else if( ( ((int)b[i] & 0xf0) == 0xe0 ) && i+2 < strbytes ) { // Start of 3-byte encoding
        if( ( ((int)b[i+1] & 0xc0) == 0x80 ) && ( ((int)b[i+2] & 0xc0) == 0x80 ) ) { // 3-byte headers
          uc = ( 4096 * ( (int)b[i] & 0x0f ) ) + ( 64 * ( (int)b[i+1] & 0x3f ) ) + ( (int)b[i+2] & 0x3f );
          if( uc > 0x7ff ) { // Valid 3-byte UTF-8 character U+800 - U+FFFF

            bwidth = 3;
          }
          else { // Overlong 3-byte UTF-8 (non-standard)

            bwidth = 3;
          }
        }
        else { // Invalid continue byte(s)
          uc = -1;
        }
      }
      else if( ( ((int)b[i] & 0xf8) == 0xf0 ) && i+3 < strbytes ) { // Start of 4-byte encoding
        if( ( ((int)b[i+1] & 0xc0) == 0x80 ) && ( ((int)b[i+2] & 0xc0) == 0x80 ) && ( ((int)b[i+3] & 0xc0) == 0x80 ) ) { // 4-byte headers
          uc = ( 262144 * ( (int)b[i] & 0x07 ) ) + ( 4096 * ( (int)b[i+1] & 0x3f ) ) + ( 64 * ( (int)b[i+2] & 0x3f ) ) + ( (int)b[i+3] & 0x3f );
          if( uc > 0xffff && uc <= 0x10ffff ) { // Valid 4-byte UTF-8 character U+10000 - U+10FFFF

            bwidth = 4;
          }
          else if( uc > 0x10ffff ) { // 4-byte UTF-8 U+110000 - U+1FFFFF (non-standard)

            bwidth = 4;
          }
          else { // Overlong 4-byte UTF-8 (non-standard)

            bwidth = 4;
          }
        }
        else { // Invalid continue byte(s)
          uc = -1;
        }
      }
      else if( ( ((int)b[i] & 0xfc) == 0xf8 ) && i+4 < strbytes ) { // Start of 5-byte encoding (non-standard)
        if( ( ((int)b[i+1] & 0xc0) == 0x80 ) && ( ((int)b[i+2] & 0xc0) == 0x80 ) && ( ((int)b[i+3] & 0xc0) == 0x80 ) && ( ((int)b[i+4] & 0xc0) == 0x80 ) ) { // 5-byte headers
          uc = ( 16777216 * ( (int)b[i] & 0x03 ) ) + ( 262144 * ( (int)b[i+1] & 0x3f ) ) + ( 4096 * ( (int)b[i+2] & 0x3f ) ) + ( 64 * ( (int)b[i+3] & 0x3f ) ) + ( (int)b[i+4] & 0x3f );
          if( uc > 0x1fffff ) { // 5-byte UTF-8 character U+200000 - U+3FFFFFF (non-standard)

            bwidth = 5;
          }
          else { // Overlong 5-byte UTF-8 (non-standard)

            bwidth = 5;
          }
        }
        else { // Invalid continue byte(s)
          uc = -1;
        }

      }
      else if( ( ((int)b[i] & 0xfe) == 0xfc ) && i+5 < strbytes ) { // Start of 6-byte encoding (non-standard)
        if( ( ((int)b[i+1] & 0xc0) == 0x80 ) && ( ((int)b[i+2] & 0xc0) == 0x80 ) && ( ((int)b[i+3] & 0xc0) == 0x80 ) && ( ((int)b[i+4] & 0xc0) == 0x80 ) && ( ((int)b[i+5] & 0xc0) == 0x80 ) ) { // 6-byte headers
          uc = ( 1073741824 * ( (int)b[i] & 0x01 ) ) + ( 16777216 * ( (int)b[i+1] & 0x3f ) ) + ( 262144 * ( (int)b[i+2] & 0x3f ) ) + ( 4096 * ( (int)b[i+3] & 0x3f ) ) + ( 64 * ( (int)b[i+4] & 0x3f ) ) + ( (int)b[i+5] & 0x3f );
          if( uc > 0x3ffffff ) { // 6-byte UTF-8 character U+4000000 - U+7FFFFFFF (non-standard)

            bwidth = 6;
          }
          else { // Overlong 6-byte UTF-8 (non-standard)

            bwidth = 6;
          }
        }
        else { // Invalid continue byte(s)
          uc = -1;
        }

      }
      else { // Invalid, possibly truncated multibyte header
        uc = -1;
      }

      if( uc >= 0 ) {
        switch( bwidth ) {
          case 1:
          printf("U+%02x: %c\n", uc, b[i]);
          i++;
          break;
          case 2:
          printf("U+%x: %c%c\n", uc, b[i], b[i+1]);
          i = i + 2;
          break;
          case 3:
          printf("U+%x: %c%c%c\n", uc, b[i], b[i+1], b[i+2]);
          i = i + 3;
          break;
          case 4:
          printf("U+%x: %c%c%c%c\n", uc, b[i], b[i+1], b[i+2], b[i+3]);
          i = i + 4;
          break;
          case 5:
          printf("U+%x\n", uc);
          i = i + 5;
          break;
          case 6:
          printf("U+%x\n", uc);
          i = i + 6;
          break;
          default:
          i++;
          break;
        }
      }

    }

    memset(b,'\0',1024);

  }

  fclose(fp);
  return 0;

}

