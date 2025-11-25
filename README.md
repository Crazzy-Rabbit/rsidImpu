# rsidImpu

**A high-performance tool to annotate GWAS summary statistics with correct rsid using dbSNP.**

`rsidImpu` is a lightweight, fast, and scalable C++ tool designed for genome-wide association study (GWAS) datasets.
It matches GWAS variants to dbSNP records using **chromosome + position + allele matching (with flip & strand complement support)** and outputs both matched and unmatched variants.

This tool is optimized for performance and can handle extremely large datasets, including:

- **15GB dbSNP** reference files (tsv or gz)
- **millions of GWAS variants**
- **parallel processing (OpenMP)**
- **gzip input support**

---

## ✨ Features

### ✔ Accurate RSID Matching

- Matches variants by **chromosome + position + allele information**
- Allows:
  - Allele flipping (A1/A2 ↔ A2/A1)
  - Strand complement matching (A↔T, C↔G)

### ✔ High Performance

- Multithreaded (OpenMP)
- Streamed reading of large files
- Zero-copy gzip reader
- Hash-based dbSNP lookup (`unordered_map`)

### ✔ Clean Output

- Matched rows → `<out>.txt`
- Unmatched rows → `<out>.txt.unmatched`
- GWAS alleles are **never modified** (A1/A2 remain as-is)

## 📦 Installation

### **Compile with Makefile**

Requirements:

- C++11 or later
- zlib
- OpenMP (optional but recommended)

### Compile manually:

```
https://github.com/Crazzy-Rabbit/rsidImpu.git

unzip rsidImpu.zip

cd rsidImpu/src 
make
```

## 🚀 Usage

### Basic example

```
rsidImpu
--gwas-summary example/gwas_test_clean.txt \
--dbsnp example/dbsnp_test.txt \
--out example/gwas_rsid.txt \
--dbchr CHR --dbpos POS --dbA1 REF --dbA2 ALT --dbrsid RSID
```

### Show help

```
rsidImpu --help
```

---

## 📥 Input Formats

### **GWAS summary statistics**

Required columns:

- CHR (or custom name)
- POS
- A1
- A2
- P (p-value)

### **dbSNP reference**

Two supported formats:

#### 1️⃣ TSV/CSV with header

Required columns:

- CHR
- POS
- REF
- ALT
- RSID

#### 2️⃣ support PLINK `.bim` or `.bim.gz`

## 📤 Output

### **Matched variants**

Written to:  out.txt\
`<original GWAS columns>` SNP

### **Unmatched variants**

Written to: out.txt.unmatched

## 📤 Acknowledgement
Thanks for the code support provided by ChatGPT.

## 📫Contract
If you have any questions, please feel free to contact us at crazzy_rabbit@163.com at any time.
