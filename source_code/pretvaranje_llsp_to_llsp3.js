const fs = require("fs");
const path = require("path");
const JSZip = require("jszip");

const VirtualMachine = require("scratch-vm");
const ScratchStorage = require("scratch-storage");

async function normalizeLLSP(inputPath, outputPath) {
    console.log("Učitavam:", inputPath);
    
    // 1. Učitaj .llsp kao ZIP
    const llspData = fs.readFileSync(inputPath);
    const llspZip = await JSZip.loadAsync(llspData);
    
    // 2. Pronađi i ekstraktiraj scratch.sb3 iz .llsp
    const scratchSb3 = llspZip.file("scratch.sb3");
    if (!scratchSb3) {
        throw new Error("scratch.sb3 nije pronađen u .llsp datoteci!");
    }
    
    const sb3Data = await scratchSb3.async("arraybuffer");
    console.log("Ekstraktiran scratch.sb3, veličina:", sb3Data.byteLength);
    
    // 3. Scratch VM setup
    const vm = new VirtualMachine();
    const storage = new ScratchStorage();
    
    // Dodaj dummy web store za assetse
    storage.addWebStore(
        [storage.AssetType.ImageVector,
         storage.AssetType.ImageBitmap,
         storage.AssetType.Sound],
        () => null
    );
    
    vm.attachStorage(storage);
    
    // 4. Učitaj scratch.sb3 u VM
    console.log("Učitavam projekt u Scratch VM...");
    await vm.loadProject(sb3Data);
    
    // 5. Spremi normalizirani projekt
    console.log("Spremam normalizirani projekt...");
    const normalizedSb3 = await vm.saveProjectSb3();
    
    // 6. Zamijeni scratch.sb3 u originalnom .llsp
    llspZip.file("scratch.sb3", normalizedSb3);
    
    // 7. Spremi kao .llsp3
    const outputData = await llspZip.generateAsync({
        type: "nodebuffer",
        compression: "DEFLATE",
        compressionOptions: { level: 9 }
    });
    
    fs.writeFileSync(outputPath, outputData);
    console.log("✔ Normaliziran i spremljen:", outputPath);
}

// Batch procesiranje - konvertiraj sve .llsp u folderu
async function batchNormalize(inputDir, outputDir) {
    if (!fs.existsSync(outputDir)) {
        fs.mkdirSync(outputDir, { recursive: true });
    }
    
    const files = fs.readdirSync(inputDir).filter(f => f.endsWith(".llsp"));
    console.log(`Pronađeno ${files.length} .llsp datoteka`);
    
    let success = 0;
    let failed = 0;
    
    for (const file of files) {
        const inputPath = path.join(inputDir, file);
        const outputPath = path.join(outputDir, file.replace(".llsp", ".llsp3"));
        
        try {
            await normalizeLLSP(inputPath, outputPath);
            success++;
        } catch (err) {
            console.error(`❌ Greška za ${file}:`, err.message);
            failed++;
        }
    }
    
    console.log(`\n=== ZAVRŠENO ===`);
    console.log(`Uspješno: ${success}`);
    console.log(`Neuspješno: ${failed}`);
}

// CLI
async function main() {
    if (process.argv.length < 3) {
        console.error("Usage:");
        console.error("  node normalize_llsp.js input.llsp [output.llsp3]");
        console.error("  node normalize_llsp.js --batch inputDir outputDir");
        process.exit(1);
    }
    
    if (process.argv[2] === "--batch") {
        const inputDir = process.argv[3];
        const outputDir = process.argv[4] || inputDir + "_normalized";
        await batchNormalize(inputDir, outputDir);
    } else {
        const input = process.argv[2];
        const output = process.argv[3] || input.replace(".llsp", ".llsp3");
        await normalizeLLSP(input, output);
    }
}

main().catch(err => {
    console.error("❌ Fatal error:", err);
    process.exit(1);
});