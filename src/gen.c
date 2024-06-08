/*
   Copyright 2024 Ryan "rj45" Sanche

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSETSYS_IMPLEMENTATION
#define STRPOOL_IMPLEMENTATION

#include "strpool.h"

#include "assetsys.h"
#include "core/structdescs.h"

const char *copyright =
  "/*\n"
  "   Copyright 2024 Ryan \"rj45\" Sanche\n"
  "\n"
  "   Licensed under the Apache License, Version 2.0 (the \"License\");\n"
  "   you may not use this file except in compliance with the License.\n"
  "   You may obtain a copy of the License at\n"
  "\n"
  "       http://www.apache.org/licenses/LICENSE-2.0\n"
  "\n"
  "   Unless required by applicable law or agreed to in writing, software\n"
  "   distributed under the License is distributed on an \"AS IS\" BASIS,\n"
  "   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or "
  "implied.\n"
  "   See the License for the specific language governing permissions and\n"
  "   limitations under the License.\n"
  "*/\n\n";

int main(int argc, char **argv) {
  char *buffer = 0;
  size_t length = 0;

  mz_zip_archive zip = {0};

  if (!mz_zip_writer_init_heap(&zip, 0, 0)) {
    fprintf(stderr, "Failed to initialize zip writer\n");
    return 1;
  }

  for (int i = 1; i < (argc - 1); i++) {
    printf("Zipping \"%s\"\n", argv[i]);
    char *archiveBuf = strdup(argv[i]);
    char *archiveFile = strstr(archiveBuf, "assets");
    if (!archiveFile) {
      fprintf(stderr, "Could not find `assets` in \"%s\"\n", argv[i]);
      return 1;
    }
    for (char *p = archiveFile; *p; p++) {
      if (*p == '\\') {
        *p = '/';
      }
    }
    if (!mz_zip_writer_add_file(
          &zip, archiveFile, argv[i], "", 0, MZ_BEST_COMPRESSION)) {
      fprintf(stderr, "Failed to add file \"%s\" to zip\n", argv[i]);
      return 1;
    }
    free(archiveBuf);
  }

  mz_zip_writer_finalize_heap_archive(&zip, (void **)&buffer, &length);

  FILE *fp = fopen(argv[argc - 1], "wt");

  fprintf(
    fp,
    "// This file was generated by gen.c from a zip of res/assets -- DO NOT "
    "EDIT\n\n");

  fprintf(fp, "unsigned int assets_zip_len = %td;\n", length);
  fprintf(fp, "const unsigned char assets_zip[] = {\n    ");
  for (int i = 0; i < length; i++) {
    fprintf(fp, "0x%02x,", buffer[i] & 0xff);
    if (i % 10 == 9) {
      fprintf(fp, "\n    ");
    }
  }
  fprintf(fp, "\n};\n");

  fclose(fp);
  mz_zip_writer_end(&zip);

  fp = fopen("src/core/structs.h", "wt");
  if (!fp) {
    fprintf(stderr, "Failed to open src/core/structs.h for writing\n");
    return 1;
  }

  fprintf(
    fp, "// This file was generated by gen.c from structdescs.h -- DO NOT "
        "EDIT\n\n");

  fprintf(fp, "%s", copyright);

  fprintf(fp, "#ifndef STRUCTS_H\n");
  fprintf(fp, "#define STRUCTS_H\n\n");

  fprintf(fp, "#include <stddef.h>\n#include <stdint.h>\n\n");
  fprintf(fp, "#include \"handmade_math.h\"\n\n");

  bool prevComment = false;
  for (int i = 0; i < sizeof(typeList) / sizeof(typeList[0]); i++) {
    bool hasComment = false;

    char *dup = strdup(typeList[i].desc);
    char *line = strtok(dup, "\n");
    while (line && line[0] != '\0') {
      if (i != 0 && !prevComment) {
        fprintf(fp, "\n");
      }
      fprintf(fp, "// %s\n", line);
      hasComment = true;
      line = strtok(NULL, "\n");
    }
    free(dup);

    fprintf(fp, "typedef %s %s;\n", typeList[i].type, typeList[i].name);
    if (hasComment) {
      fprintf(fp, "\n");
    }
    prevComment = hasComment;
  }

  struct {
    const char *name;
    size_t num;
    const StructDesc *descs;
  } lists[] = {
    {"helperDescs", sizeof(helperDescs) / sizeof(helperDescs[0]), helperDescs},
    {"structDescs", sizeof(structDescs) / sizeof(structDescs[0]), structDescs},
  };

  for (int didx = 0; didx < 2; didx++) {
    const char *name = lists[didx].name;
    size_t num = lists[didx].num;
    const StructDesc *descs = lists[didx].descs;

    for (int i = 0; i < num; i++) {
      const StructDesc *desc = &descs[i];

      bool skip = false;
      for (int j = 0; j < sizeof(externTypes) / sizeof(externTypes[0]); j++) {
        if (strcmp(desc->name, externTypes[j]) == 0) {
          skip = true;
        }
      }
      if (skip) {
        continue;
      }

      char *dup = strdup(desc->desc);
      char *line = strtok(dup, "\n");
      while (line && line[0] != '\0') {
        if (i != 0 && !prevComment) {
          fprintf(fp, "\n");
        }
        fprintf(fp, "// %s\n", line);
        line = strtok(NULL, "\n");
      }
      free(dup);

      fprintf(fp, "typedef struct %s {\n", desc->name);
      for (int j = 0; j < desc->numFields; j++) {
        if (j != 0) {
          fprintf(fp, "\n");
        }

        char *dup = strdup(desc->descs[j]);
        char *line = strtok(dup, "\n");
        while (line && line[0] != '\0') {
          if (i != 0 && !prevComment) {
            fprintf(fp, "\n");
          }
          fprintf(fp, "  // %s\n", line);
          line = strtok(NULL, "\n");
        }
        free(dup);

        fprintf(fp, "  %s %s;\n", desc->types[j], desc->names[j]);
      }
      fprintf(fp, "} %s;\n\n", desc->name);
    }
  }

  fprintf(
    fp, "// A description of a struct for reflection.\n"
        "typedef struct StructDesc {\n"
        "  const char *name;\n"
        "  size_t size;\n"
        "  size_t numFields;\n"
        "  size_t *offsets;\n"
        "  size_t *sizes;\n"
        "  const char **names;\n"
        "  const char **types;\n"
        "} StructDesc;\n\n");

  fprintf(
    fp, "extern const StructDesc helperDescs[%zu];\n",
    sizeof(helperDescs) / sizeof(helperDescs[0]));
  fprintf(
    fp, "extern const StructDesc structDescs[%zu];\n\n",
    sizeof(structDescs) / sizeof(structDescs[0]));

  fprintf(fp, "#endif // STRUCTS_H\n");
  fclose(fp);

  fp = fopen("src/core/structdescs.c", "wt");
  if (!fp) {
    fprintf(stderr, "Failed to open src/core/structdescs.c for writing\n");
    return 1;
  }

  fprintf(
    fp, "// This file was generated by gen.c from structdescs.h -- DO NOT "
        "EDIT\n\n");

  fprintf(fp, "%s", copyright);

  fprintf(fp, "#include \"core/structs.h\"\n\n");

  for (int didx = 0; didx < 2; didx++) {
    const char *name = lists[didx].name;
    size_t num = lists[didx].num;
    const StructDesc *descs = lists[didx].descs;

    if (didx != 0) {
      fprintf(fp, "\n");
    }

    fprintf(fp, "const StructDesc %s[%lu] = {\n", name, num);
    for (int i = 0; i < num; i++) {
      const StructDesc *desc = &descs[i];
      fprintf(fp, "  {\n");
      fprintf(fp, "    .name = \"%s\",\n", desc->name);
      fprintf(fp, "    .size = %zu,\n", desc->size);
      fprintf(fp, "    .numFields = %lu,\n", desc->numFields);
      fprintf(fp, "    .offsets = (size_t[]){\n      ");
      for (int j = 0; j < desc->numFields; j++) {
        if (j != 0) {
          fprintf(fp, " ");
        }
        fprintf(fp, "%zu,", desc->offsets[j]);
      }
      fprintf(fp, "\n    },\n");
      fprintf(fp, "    .sizes = (size_t[]){\n      ");
      for (int j = 0; j < desc->numFields; j++) {
        if (j != 0) {
          fprintf(fp, " ");
        }
        fprintf(fp, "%zu,", desc->sizes[j]);
      }
      fprintf(fp, "\n    },\n");
      fprintf(fp, "    .names = (char const *[]){\n");
      for (int j = 0; j < desc->numFields; j++) {
        fprintf(fp, "      \"%s\",\n", desc->names[j]);
      }
      fprintf(fp, "    },\n");
      fprintf(fp, "    .types = (char const *[]){\n");
      for (int j = 0; j < desc->numFields; j++) {
        fprintf(fp, "      \"%s\",\n", desc->types[j]);
      }
      fprintf(fp, "    },\n");
      fprintf(fp, "  },\n");
    }
    fprintf(fp, "};\n");
  }

  fclose(fp);

  return 0;
}