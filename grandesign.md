# LANUN JALADRI: Tides of Nusantara — README & Grand Design

> **Fungsi dokumen:** README proyek, sumber utama grand design, dan checklist TODO implementasi.
>
> **Status:** fitur yang tercantum sebagai fondasi/prototipe ditemukan pada audit snapshot proyek, tetapi belum tentu lengkap atau teruji. Getter kapal baru ditambahkan dan telah dikompilasi berhasil. Keputusan target UE 5.8.4 Source/Win64, renderer DX12/SM6, scope Single-Player Offline-First, dan pembekuan mobile telah ditetapkan. Source build, cook, packaging, save backup, dan skala dunia telah divalidasi melalui build UE 5.8.4 yang berhasil. Semua kotak TODO sengaja belum dicentang sampai diverifikasi.

## Status cepat

- **Genre:** Open-World Maritime Action-RPG / Tactical Sandbox.
- **Tahap:** perencanaan dan prototipe; belum menjadi game end-to-end yang tervalidasi.
- **Engine target:** Unreal Engine 5.8.4 Source Build; `.uproject` memakai GUID asosiasi lokal yang perlu dipastikan menunjuk ke build tersebut.
- **Platform sasaran:** PC Windows (Win64), target workstation RTX 2000 Ada. Android/iOS **FROZEN** sampai Shipping PC stabil dan dapat dimainkan.
- **Mode permainan:** 100% Single-Player Offline-First. Fitur online dan cross-save ditangguhkan.
- **Build sasaran:** Development untuk editor/testing; Shipping untuk packaging rilis. Console dan debug symbols harus dipastikan tidak ikut pada Shipping lewat konfigurasi/build pipeline dan pemeriksaan hasil, bukan diasumsikan dari konfigurasi Editor.
- **Renderer sasaran:** DX12/SM6, Lumen GI/reflections dengan Hardware RT, VSM, TSR, Substrate, volumetric fog dan shader pipeline cache; belum ada hasil build/cook untuk membuktikan konfigurasi ini bekerja.
- **Mata uang desain:** Rupiah sebagai satu-satunya mata uang gameplay.
- **Progresi desain:** level 1–100 untuk Hero, Companion, satwa, dan markas.
- **GAS skill tree:** taxonomy tag untuk skill Hero terdaftar di `DefaultGameplayTags.ini`; code ability `Tangkas Salawaku` (parry/counter) dan struktur `ULanunAttributeSetBase` sudah ada dalam rancangan C++, namun unlock, poin, input skill baru, efek gameplay, dan integrasi AbilitySpec belum teruji di Editor/build gameplay.
- **Prioritas kerja saat ini:** stabilkan proyek, peta startup dan input; lalu buat vertical slice pelarian hingga pertempuran laut pertama.
- **Checklist TODO:** [Urutan implementasi](#10-urutan-implementasi-todo).

### TODO tingkat atas

- [ ] Verifikasi asosiasi dan compile proyek menggunakan source build UE 5.8.4 yang ditetapkan.
- [ ] Ganti peta startup yang hilang dengan peta gameplay yang valid.
- [ ] Selesaikan vertical slice cerita dan gameplay yang punya awal, tengah, akhir, UI, serta save lokal.
- [ ] Buktikan loop kapal, combat, ekonomi Rupiah, dan quest dengan tes/playtest.
- [ ] Verifikasi Shipping Win64 sebelum mempertimbangkan pembukaan kembali scope mobile atau fitur online/cross-save yang saat ini dibekukan/ditangguhkan.

## 1. Visi dan identitas game

**Genre:** Open-World Maritime Action-RPG / Tactical Sandbox  
**Inspirasi desain:** kebebasan petualangan bajak laut, perdagangan dan diplomasi maritim, serta simulasi faksi dan komando armada.  
**Setting:** sejarah alternatif maritim Asia Tenggara/Nusantara.  
**Target:** PC Windows Win64 saja untuk vertical slice dan rilis awal. Android/iOS berstatus **FROZEN**. Arsitektur 100% Single-Player Offline-First; online, multiplayer dan cross-save berstatus **DEFERRED**, bukan sasaran implementasi vertical slice.

Pemain memulai sebagai penyintas kehancuran desa pesisir. Dari perahu kecil, kru pelarian, dan markas tersembunyi, pemain membangun armada dan jaringan pelabuhan. Perjalanan berujung pada pilihan: membebaskan dan menyatukan wilayah, bersekutu dengan kerajaan lokal, membeli hak kelola kampung, atau menguasai pelabuhan dengan kekuatan. Kekuasaan kemudian diuji oleh rakyat, Companion, faksi, ekonomi, dan generasi penerus.

### Prinsip desain

1. **Laut adalah ruang bermain utama:** navigasi, cuaca, perdagangan, pertempuran, dan eksplorasi pesisir saling terhubung.
2. **Pilihan ekspansi punya konsekuensi:** aliansi, pembelian, dan penaklukan masing-masing memiliki manfaat dan biaya; tidak ada satu jalur yang selalu terbaik.
3. **Kru adalah karakter, bukan angka:** Companion, kapten, dan awak memiliki fungsi, loyalitas, risiko, serta cerita.
4. **Dunia bereaksi:** popularitas, reputasi, harga, blokade, konflik, dan event mengubah peluang pemain.
5. **Progresi membuka pilihan:** level dan fasilitas memperluas strategi, bukan hanya menaikkan angka.
6. **Konteks budaya dihormati:** tradisi, fauna, komunitas, dan sejarah diperlakukan dengan riset; unsur mistis adalah folklore fiksi dalam dunia game.

## 2. Batas audit: implementasi saat ini

Audit snapshot menemukan fondasi C++ untuk beberapa sistem, tetapi belum menemukan dunia game siap dimainkan yang sesuai grand design. Informasi berikut bukan klaim bahwa fitur telah selesai.

### Fondasi/prototipe yang ditemukan

| Area | Bukti di proyek | Batas saat audit |
|---|---|---|
| UE dan plugin | `Tides_Of_Nusantara.uproject` mencantumkan Water, Gameplay Abilities, StateTree, dan Mass AI. | Plugin aktif tidak berarti sistemnya sudah dipakai. Water Body, buoyancy kapal, World Partition/Data Layers, dan AI lalu lintas belum terbukti terintegrasi. |
| Hero dan combat dasar | Kelas Hero memakai Enhanced Input dan Gameplay Ability System; ada ability serangan Keris dan Senapan Pasak. | Belum ada keseluruhan parkour, stealth, stance, montase combo, atau combat RPG seperti rencana. |
| Kapal | `ShipPawnBase` dan `ShipMovementComponent` mengatur input layar/kemudi serta gaya dan torsi dasar. | Belum ada implementasi buoyancy kapal terverifikasi, armada pengiring aktif, atau sistem kargo yang memengaruhi draft. |
| Naval combat | `NavalCombatSubsystem` berisi grup meriam, cooldown, hit trace, dan kalkulasi damage. | Boarding, target selection, kerusakan komponen, dan cooldown belum tampak sebagai loop end-to-end. |
| Armada/perwira | `ShipFleetSubsystem` dan `CrewOfficerSubsystem` menyimpan data kapal, perwira, dan bonus. | Roster contoh belum sama dengan roster cerita; belum ada fleet command dan pengelolaan Companion lengkap. |
| Waktu/cuaca | Kalender, musim muson, dan preset cuaca ada. | Pengaruhnya ke laut, visual, kapal, dan gameplay belum terverifikasi sebagai sistem terpadu. |
| Dagang/wilayah | Ada pasar komoditas dasar dan pembelian kampung. | Memakai `PlayerGold` float, bukan satu ekonomi Rupiah; belum ada biaya pelabuhan, inventaris cargo, utang bercabang, pajak, atau trade route otomatis. |
| Save lokal | `ULanunSaveGameSubsystem` menyimpan waktu, posisi, wilayah, kapal, perwira, dan pet melalui satu slot Unreal. | Nama slot saat ini `LanunSaveSlot_0`; backup `.bak`, schema version/migration, jalur pemulihan, wallet Rupiah dan banyak data gameplay belum diimplementasikan. Cloud sync tidak ditemukan dan memang di luar scope aktif. |
| Satwa companion | Subsistem pet memiliki contoh pet, XP, loyalitas, perilaku, dan tahap tumbuh. | Bukan sistem fauna/mount dan progresi satwa Level 1–100 yang ditargetkan. |
| AI/dialog | AI punya persepsi sight dan faksi sederhana; subsistem dialog mengirim request ke Ollama lokal. | Belum ada simulasi geopolitik, rutinitas NPC, atau rantai quest. |

### Blocker dan batas validasi yang diketahui

- Konfigurasi `DefaultEngine.ini` menunjuk peta `/Game/Maps/Lvl_Nusantara_Main`, tetapi peta itu tidak ditemukan pada audit. Peta yang ditemukan adalah peta template.
- Folder konten `Maps`, `Ships`, `Environment`, `UI`, `Blueprints`, `Data`, dan `Lanun` tidak memiliki aset `.umap`/`.uasset` gameplay saat audit. Aset input/touch template tersedia.
- `GetBoardState()` dan `GetShipMovementComponent()` sebelumnya dipanggil tetapi belum dideklarasikan. Getter BlueprintPure kini ditambahkan di `ShipPawnBase.h`; **kompilasi UE 5.8.4 telah berhasil** setelah tambahan include `ShipFleetSubsystem.h`, `CalendarWorldSubsystem.h`, `LanunCharacterHeroBase.h`, `InputAction.h`, `InputActionValue.h`, `Json` module di `Build.cs`, serta perbaikan binding Enhanced Input dan parameter shadowing `ModelName`. `EBordState` adalah nama enum saat ini dan direncanakan dirapikan menjadi `EShipBoardState`; cek Blueprint/data asset references dan gunakan CoreRedirect jika ditemukan sebelum menghapus nama lama.
- Audit tidak menemukan `RivalNemesisSubsystem.h/.cpp`, `NpcDialogHttpSubsystem.h/.cpp` versi usulan, schema `FOfficerLegacyState`, atau utility XP final. Potongan kode yang ditempel dalam rancangan bukan bukti file telah dibuat atau dikompilasi.
- Build UE tidak dijalankan karena Unreal Editor/Build Tool tidak tersedia di lingkungan audit. Pemeriksaan editor yang tersedia bukan pengganti build.
- Direktori kerja saat audit bukan Git repository; perubahan antarversi tidak dapat dipastikan.

### Kontrak implementasi teknis (rencana)

- **Mata uang:** seluruh saldo dan transaksi menggunakan integer `int64` dengan nama domain `PlayerRupiah`; emas/perak fisik adalah item komoditas, bukan mata uang kedua. Satu sumber saldo otoritatif harus dipakai bersama oleh Hero, trade, buyout, repair, save, UI dan quest. Hindari saldo duplikat independen pada setiap subsystem.
- **Migrasi nama API prototipe:** selagi aset gameplay belum terbukti bergantung pada nama prototipe, rencanakan penggantian `PlayerGold` menjadi API dompet terpusat `int64 PlayerRupiah`, `EBordState` menjadi `EShipBoardState`, dan label faksi runtime `VOC_Dutch` menjadi `WesternTradeCompany`/“Kongsi Dagang Barat”. Audit seluruh pemanggil serta Blueprint serialized references; pakai redirect/migrasi bila perlu. Ini belum dilakukan dan tidak boleh sekadar rename sebagian.
- **Batas arsitektur uang:** `ULanunAttributeSetBase` menangani atribut gameplay/GAS; jangan menjadikan AttributeSet salinan saldo kedua. Jika integrasi UI/GAS mengharuskan saldo di sana, tentukan satu pemilik state otoritatif dan sinkronisasi satu arah/transaksi yang atomik. Jangan pakai `float` untuk menyimpan Rupiah; tentukan aturan pembulatan harga dan lindungi operasi dari overflow/underflow.
- **Ship API:** tambahkan getter BlueprintPure untuk movement component dan boarding state yang sedang dipakai. Gunakan satu nama enum/state konsisten (`EBordState` yang ada atau migrasi menyeluruh yang disengaja); jangan membuat `EShipBoardState`/`CurrentBoardState` paralel sementara kode memakai `EBordState`/`BoardState`.
- **Nemesis:** `URivalNemesisSubsystem` perlu state persistensi lintas level/save. WorldSubsystem dapat menyediakan akses runtime per world, tetapi sumber data kampanye perlu disimpan/dipulihkan; memori taktik memakai encounter rolling/recency, bukan bool permanen.
- **Dialog NPC:** subsystem Ollama yang terdeteksi sekarang adalah `UNpcDialogHttpSubsystem` WorldSubsystem; enum `ENpcFactionType` masih memiliki identifier `VOC_Dutch`. Usulan mengubahnya menjadi GameInstanceSubsystem/kelas bernama berbeda adalah keputusan migrasi, bukan perubahan yang sudah terjadi. Pertahankan API/aset Blueprint atau siapkan redirect dan rencana migrasi. Ubah label yang dilihat pemain menjadi “Kongsi Dagang Barat” atau migrasikan identifier secara menyeluruh; endpoint lokal, kegagalan request, JSON response, NPC ID, timeout, dan opsi mematikan dialog AI harus ditangani eksplisit.
- **XP:** implementasikan satu fungsi utility deterministik dengan input level 1–100, return 0 pada level invalid/cap, hitung dengan presisi yang cukup (`double` sebelum floor), dan tes tabel threshold serta cumulative XP. Fungsi ini belum ada pada audit.
- **Status checklist:** matriks `[X]` dalam catatan eksternal yang belum diverifikasi tidak boleh menandai subsystem, perbaikan audit, AssetManager, input switching, build, atau header sebagai selesai. Gunakan checklist di dokumen ini dan Definition of Done; UE 5.8.4 hanya boleh dicatat setelah project association/build benar-benar membuktikannya.
- **Plugin vs module:** `.uproject` mengaktifkan plugin; `Build.cs` mendeklarasikan module dependency. `MassGameplay` adalah nama plugin, bukan module dependency, sehingga tidak boleh ditulis dalam `Build.cs`. Module Mass pada `Build.cs` saat ini meliputi `MassEntity`, `MassCrowd`, dan `MassActors`; nama plugin untuk enablement harus diverifikasi dari descriptor UE 5.8.4. Jangan menyalin `"MassEntity"`/`"MassActors"` ke `.uproject` sebagai plugin hanya karena namanya ada di `Build.cs`.
- **Kebijakan dependency:** plugin core engine yang dibutuhkan adalah GAS (`GameplayAbilities`), Enhanced Input, Water (`Water`, `WaterAdvanced`), StateTree/Gameplay StateTree, Mass serta UMG. Plugin `StateTree`/`GameplayStateTree`/Mass yang di-enable dalam `.uproject` dan module `StateTreeModule`/`GameplayStateTreeModule`/`MassEntity`/`MassCrowd`/`MassActors` pada `Build.cs` diverifikasi terpisah terhadap instalasi source build UE 5.8.4. Plugin pihak ketiga ditangguhkan sampai vertical slice lulus kualifikasi. Manifest sampel dalam catatan eksternal bukan pengganti validasi descriptor dan dependency proyek.
- **Renderer/platform:** `DefaultEngine.ini` mengatur DX12 sebagai RHI default, target D3D12 SM6, Lumen/HWRT, VSM, TSR, Substrate dan volumetric fog. `r.Water.EnableOceanBuoyancy` tidak ditambahkan sebagai CVar karena buoyancy harus dikonfigurasi pada plugin/komponen Water yang benar-benar digunakan; `r.DirectX.DX12` juga tidak dipakai karena RHI ditetapkan melalui WindowsTargetSettings. `r.Substrate.BytesPerPixel` dan `r.TargetPreexposure` sengaja tidak ditambahkan sebelum validitas/efeknya dikonfirmasi terhadap source build.

## 3. Dunia, geografi, dan sejarah

### Cakupan target

Wilayah laut dan pesisir penuh tetap menjadi visi jangka panjang setelah slice; konten pertama dibatasi pada Selat Makassar dan Kepulauan Spermonde:

- **Cakupan vertical slice:** sektor Selat Makassar dan Kepulauan Spermonde dengan luas gameplay target sekitar 100 km² (10 × 10 km), 2 pelabuhan:
  - **Bandar Somba Opu:** hub perdagangan, quest utama, dan transaksi kargo pasar gelap.
  - **Teluk Tersembunyi Spermonde:** markas awal untuk merakit Lontara Hitam dan menguji sistem Beli Kampung.
- **Dynamic Spatial Scaling:** area lokal pelabuhan/pesisir/pertempuran/boarding memakai rasio representasi target 1:10–1:15; laut lepas antar-sektor 1:100–1:150, dengan pelayaran samudra antarpulau 5–8 menit real-time. Ini abstraksi jarak/perpindahan antar-mode atau sektor, bukan scaling transformasi actor/physics secara live. Transisi harus mempertahankan heading, kecepatan, waktu, cuaca dan state armada.
- **Sektor:** streaming/peta sektor sekitar 10 × 10 km sebagai target authoring dan pengendalian memori/VRAM, bukan jaminan batas penggunaan VRAM. Kepadatan konvoi, patroli Salisipan, karang, landmark dan treasure harus diuji di PC target agar tidak menjadi empty void atau membebani CPU/GPU.
- **Fast travel:** hanya antara pelabuhan dengan fasilitas markas yang sudah dikuasai; membayar ransum dan upah kru serta memajukan waktu dan simulasi risiko rute.
- Skala dan target waktu perjalanan menjadi baseline desain yang diputuskan, tetapi tetap perlu prototipe navigasi/streaming dan profiling sebelum diklaim terimplementasi atau berkinerja sesuai.
- Ekspansi ke Melaka/Singapura, Jawa, Bali, Kalimantan, Maluku/Banda, Nusa Tenggara, Timor, Arafura dan Papua/PNG berada di luar area slice awal.

### Konsistensi sejarah

Tahun cerita dikunci pada **1521**, dengan sejarah alternatif untuk memungkinkan Majapahit tetap menjadi kekuatan aktif dan perebutan rempah berjalan sesuai narasi. Protagonis adalah karakter tetap: penyintas desa pesisir yang tumbuh menjadi Nakhoda; kustomisasi terbatas pada nama gelar, pakaian (Kawaca, Tanjak, Baju Kurung) dan jimat Wafak. Faksi asing dalam UI/Codex dinamai **Kongsi Dagang Barat**; hindari nama VOC yang berpotensi anakronistis. Hubungan Majapahit, Demak, Portugis, kerajaan lokal dan jaringan pedagang dijelaskan sebagai timeline alternatif. Rupiah adalah mata uang gameplay, bukan klaim akurasi moneter era tersebut.

### Arsitektur dunia yang ditargetkan

- Unreal Engine 5; World Partition dan Data Layers untuk streaming wilayah, jika sesuai struktur peta.
- Large World Coordinates mendukung koordinat berskala besar, tetapi bukan pengganti desain streaming dan optimasi.
- Water Body, sistem gelombang, buoyancy dan kapal harus diukur melalui prototipe. Chaos Buoyancy tidak otomatis tersedia hanya dengan mengaktifkan plugin Water.
- PCG untuk vegetasi/lingkungan bila hasil visual, determinisme, dan biaya runtime sesuai.
- Profil konten, pencahayaan, shader, serta LOD desktop adalah scope aktif; profil mobile baru dipertimbangkan setelah status FROZEN dibuka kembali. Nanite/Lumen bukan asumsi universal untuk perangkat mobile.

### KPI playtest sektor dan workstation

Baseline pengukuran untuk sektor 100 km² di workstation RTX 2000 Ada; ini target uji, bukan hasil yang sudah tercapai:

| Parameter | Baseline target | Batas/validasi |
|---|---|---|
| GPU VRAM | Maksimum 7,5 GB untuk game pada 1080p | Profil saat berlayar, pelabuhan, badai dan encounter; ukur peak, shader cache dan streaming, bukan hanya rata-rata. Sisakan sisa VRAM untuk OS/aplikasi. Ollama `qwen2.5-coder:14b` berdampingan adalah eksperimen opsional: jangan menjanjikan model itu muat/berjalan di sisa 8,5 GB tanpa menguji quantization/offload; game tidak boleh bergantung pada LLM lokal. |
| Editor framerate | 60 FPS pada 1080p, Scalability High, TSR Quality | Ukur GPU/CPU frame time dalam Editor dan packaged Development; catat hitch/1% low terpisah. Editor 60 FPS bukan jaminan Shipping. |
| Pelayaran lokal | Pesisir ke karang 2–4 menit real-time | Uji di rute referensi dengan angin/cuaca, muatan dan arah perjalanan tetap tercatat. |
| Transit laut lepas | 5–8 menit real-time antar-sektor | Ocean scale target 1:100; pemajuan waktu/fuel/logistik dan transisi sektor harus konsisten. |
| Encounter laut | Satu event kira-kira setiap 90 detik atau 1,5–2 mil laut | Jadikan pemicu waktu/jarak satu aturan gabungan agar tidak menggandakan encounter; cooldown, kepadatan aman dan opsi menghindar wajib. |
| Fast travel | `SektorDistance × 500 Rp + CrewCount × DailyRationCostPerCrew` | Harga akhir integer Rupiah, ditampilkan sebelum konfirmasi; biaya/ransum hanya didebit sekali. Majukan kalender dan resolusikan risiko rute. |

Kepadatan konvoi, patroli Salisipan, karang, landmark dan treasure harus membangun siluet gameplay yang terbaca tanpa memenuhi seluruh laut dengan encounter acak.

## 4. Cerita dan struktur quest

### Main Questline: Dari Pelarian hingga Legenda Samudra

Struktur tujuh babak memperkenalkan subsistem bertahap melalui tujuan naratif. Nama class dan subsystem di bawah adalah sasaran integrasi; penyebutan tidak menyatakan bahwa fitur sudah selesai. Questline penuh menjelajahi Nusantara; vertical slice awal dibatasi pada sektor Selat Makassar/Spermonde dan dua pelabuhan yang ditetapkan. Babak yang membutuhkan wilayah di luar sektor tidak termasuk deliverable slice.

#### Quest 1 — Hujan Api & Pelarian dari Pesisir

- **Cerita:** desa pesisir dihantam artileri asing. Protagonis bertahan di tengah kekacauan dan mencari jalan ke dermaga.
- **Belajar:** `IMC_OnFoot` dengan `IA_Move`, `IA_Look`, `IA_Jump`, interaksi `IA_Interact` (E), stamina sprint/regen dan health melalui GAS/`ULanunAttributeSetBase`, lompatan melewati puing, berenang di air dangkal, dan satu aksi combat/defense sederhana untuk menghadapi ancaman.
- **Akhir:** berinteraksi dengan sampan kecil di dermaga dan melarikan diri.
- **Prasyarat implementasi:** input/mapping, interaksi, stamina, dan berenang harus lebih dahulu berfungsi; hindari memperkenalkan subsistem ekonomi atau fleet di prolog.

#### Quest 2 — Merakit Saksi Samudra

- **Cerita:** terdampar di teluk tersembunyi, protagonis menemukan Daeng Rannu terluka dan membantunya membangun perahu pertama, **Lontara Hitam** (Padewakang/Phinisi; kelas final ditetapkan konsisten).
- **Belajar:** mengumpulkan kayu ulin, kain layar dan besi; workbench; merekrut/menugaskan Daeng Rannu sebagai First Mate/Nakhoda; mengendalikan layar, arah angin dan kemudi menggunakan `AShipPawnBase`, `UShipMovementComponent`, dan `UCrewOfficerSubsystem`.
- **Akhir:** Lontara Hitam diluncurkan dan pemain menyelesaikan uji layar/kemudi di perairan aman.
- **Konsistensi data:** ID perwira `OFFICER_Rannu` hanya digunakan setelah asset/data perwira dengan ID tersebut ditambahkan; jangan memakai ID contoh berbeda secara tersirat.

#### Quest 3 — Bisik-Bisik Somba Opu

- **Cerita:** pemain berlayar ke Bandar Somba Opu, bertemu Putri Ratna dan mencari perbekalan serta izin memasang artileri.
- **Belajar:** perpindahan `IMC_OnFoot` ↔ `IMC_Sailing`; perdagangan dan inventaris cargo melalui `UTradeEconomySubsystem`; harga komoditas Pala/Cengkeh dipengaruhi kalender/muson; pasang Lantaka di haluan dan Meriam Cetak di sisi kapal.
- **Akhir:** jual komoditas awal, beli/pasang artileri minimum, dan berlatih membidik sasaran latihan.
- **Ekonomi:** seluruh transaksi menggunakan **Rupiah**, bukan “koin emas” atau `PlayerGold`. Jika narasi memerlukan koin fisik, jelaskan itu sebagai barang/komoditas, bukan mata uang kedua.

#### Quest 4 — Api di Laut Sulu

- **Cerita:** pemain menerima distress call dan menavigasi celah karang Salisipan untuk menyelamatkan kapal Panglima Jarang.
- **Belajar bertahap:** baca kedalaman/ancaman karang; latihan target lock dan arah kapal; eksekusi `FireBroadside`, falloff jarak dan `CalculateNavalDamage`; pilih sasaran/objektif; lanjutkan ke boarding dan combat dek dengan ability Keris/Senapan Pasak.
- **Objektif:** selamatkan Jarang dengan **melumpuhkan** kapal penyerang (misalnya tiang/kemudi) atau menenggelamkannya. Boarding mengharuskan kapal target masih dapat dinaiki; jangan mewajibkan menenggelamkan target sebelum boarding.
- **Fleet command:** perkenalkan satu perintah escort sederhana bila kapal pengiring sudah dibuka; formasi penuh dipelajari kemudian.
- **Akhir:** Jarang bergabung sebagai Juru Meriam.

#### Quest 5 — Pemburu di Hutan Arafura

- **Cerita:** bersama Kono, pemain menyusuri rawa/sungai Papua untuk membebaskan masyarakat pesisir yang ditawan.
- **Belajar:** kenalkan **satu pet** dan satu perilaku (`Guard` atau `UtilityHunt`) melalui `UPetCompanionSubsystem`; lanjutkan ke satu keputusan wilayah (membebaskan, bersekutu, atau memulai proses kepemilikan) dan konsekuensi reputasi yang terlihat.
- **Buyout:** bila pemain memilih membeli kampung, quest pendamping **“Belenggu Suratan Utang”** wajib dituntaskan sebelum panel Beli Kampung aktif; setelah bebas utang, pembelian hak kelola tetap memerlukan Rupiah. Pembebasan warga/penyelesaian konflik tidak otomatis sama dengan membeli wilayah.
- **Pacing:** jangan mengenalkan pet progression penuh, utang, buyout, passive income, mining, popularity, dan governor sekaligus. Sisakan sistem lanjutan untuk side quest dan babak berikutnya.
- **Akhir:** warga dibebaskan dan Kono direkrut; kepemilikan/pendapatan wilayah hanya terjadi jika pemain menyelesaikan jalur yang sesuai.

#### Quest 6 — Perang Badai Selat Makassar

- **Cerita:** armada musuh memblokade Selat Makassar saat muson barat. Pemain memimpin armada lokal menghadapi flagship ekspedisi.
- **Belajar:** `UMonsoonWeatherSubsystem` mengubah cuaca, gelombang, kabut dan angin; efek cuaca wajib terhubung ke dunia dan kapal, bukan hanya mengganti enum. Pemain menerapkan perbaikan lambung dan menetapkan flagship/escort melalui `UShipFleetSubsystem`.
- **Persiapan kurva belajar:** formasi penuh (`Line of Battle`, `Merchant Escort`, `Free Hunt`) diperkenalkan lewat misi pengawalan/latihan sebelum atau pada awal Q6. Q6 menguji pemahaman itu, bukan memperkenalkan seluruh kontrol armada pertama kali di tengah badai.
- **Akhir:** lumpuhkan flagship musuh dan amankan jalur. Badai memberi tantangan dengan telegraph/peringatan yang terbaca, bukan kerusakan acak tak terhindarkan.

#### Quest 7 — Mahkota Samudra & Legacy

- **Cerita:** pemain menentukan tatanan politik akhir: Federasi Samudra Bebas, pelindung kesultanan/kerajaan sekutu, atau kekuasaan terpusat dengan biaya politik.
- **Belajar:** mastery jaringan pelabuhan, konsekuensi pilihan faksi, dan evaluasi akhir.
- **Legacy:** `ULanunSaveGameSubsystem` menjadi sasaran integrasi untuk pensiun/New Game+ dan pewarisan aset yang disepakati: kapal/aset terpilih, blueprint, reputasi Companion dan pet sesuai aturan Legacy yang tampil di UI. Legacy dipisahkan dari save biasa.
- **Akhir:** narasi utama selesai, gelar **Mahkota Samudra** terbuka, lalu pemain memilih melanjutkan sandbox atau memulai generasi penerus.
- **Perlindungan progres:** sebelum konfirmasi pensiun, tampilkan daftar yang diwariskan, hilang, atau di-reset; sediakan konfirmasi eksplisit dan jangan menimpa save lama secara diam-diam.

### Prinsip kurva belajar quest

- Satu babak memperkenalkan sedikit sistem baru; misi sampingan menjadi tempat untuk memperdalam fitur opsional.
- Urutan umum: **coba aman → gunakan dalam situasi ringan → kombinasikan di situasi berisiko → uji mastery**.
- Tutorial memakai UI diegetik/kontekstual bila memungkinkan, dengan opsi bantuan dan pengulangan.
- Setiap fitur baru harus punya feedback yang jelas, gagal dengan aman, dan tidak mengharuskan pemain memahami nama class C++.
- Side quest “Belenggu Suratan Utang” tersedia setelah konflik kampung diperkenalkan, sekitar Q5; penyelesaiannya membuka buyout, bukan memaksa seluruh pemain membeli kampung.
- Unlock sistem dan hadiah tidak boleh mengasumsikan subsystem sudah tersedia hanya karena namanya muncul dalam dokumen.

### Questline wilayah dan Companion

Setiap quest Companion memperkenalkan wilayah, relasi, pilihan dan sistem yang kemudian dipakai kembali. Hadiah berikut adalah target desain; nama subsystem menunjukkan sasaran integrasi, bukan implementasi yang sudah tersedia.

#### Companion inti

| Companion / quest | Tahapan utama | Hadiah rancangan dan integrasi |
|---|---|---|
| **Daeng Rannu — “Sumpah Siri’ di Selat Makassar”** | Selidiki pengkhianatan kapal keluarga di Somba Opu; buru konvoi malam melewati karang Spermonde; boarding kapal pusaka; duel kehormatan di bawah badai muson. Latar dan pilihan menghormati nilai *Siri’ na Pacce*. | `Tactical Ramming`: +60% collision damage dan -40% kerusakan balasan. `Kemudi Siri’`: +25% rudder torque. Target integrasi: `UNavalCombatSubsystem`, `UShipFleetSubsystem`. |
| **Panglima Jarang — “Darah di Karang Salisipan”** | Bongkar rencana serangan di Jolo; pertahankan benteng Karang Salisipan dalam siege malam memakai obor laut dan Lela; infiltrasi markas untuk merebut cetak biru meriam. | `Salisipan Broadside`: cooldown tembakan -50% selama 12 detik, dengan heat/wear sebagai trade-off. `Aura Artileri Tausug`: +15% cannon damage dan +20% projectile velocity untuk armada. Target: `UNavalCombatSubsystem`, `UCrewOfficerSubsystem`. |
| **Putri Ratna — “Bayangan di Selat Melaka”** | Menyusup ke pesta saudagar; bangun jaringan agen di tiga pelabuhan; intersep manifes dan sabotase kargo tanpa memicu perang terbuka. | `Akses Pasar Gelap`: membuka barang embargo dengan diskon rancangan 20%. `Margin Smuggler`: +15% harga jual pala/cengkeh di pelabuhan independen. Target: `UTradeEconomySubsystem`. |
| **Kono — “Darah Emas Rimba Papua”** | Telusuri sungai dan bakau Papua; serang pos pembalakan/tambang liar; usir komandan ekspedisi dari situs suci. | `Pos Tambang Emas`: produksi ore pasif mingguan melalui kalender, hasil dicatat sebagai Rupiah hanya setelah pengolahan/penjualan yang ditetapkan. `Serangan Sumpitan Racun`: ability darat dengan DoT serta pengurangan stamina/kecepatan hingga 50% sebagai nilai awal untuk balancing. Target: territory/economy, `UCalendarWorldSubsystem`, `ULanunGameplayAbilityBase`. |

#### Companion tambahan

| Companion / spesialisasi | Latar dan representasi | Kemampuan rancangan | Target integrasi |
|---|---|---|---|
| **Sultan Baab — Marine Engineer / Shipwright** | Ternate/Tidore; keahlian galangan, Kora-kora, dan kayu besi. Validasi nama, masa, dan konteks budaya sebelum naskah final. | Reparasi lambung bertahap di luar pertempuran; +25% ketahanan terhadap tembakan meriam melalui Armor Kayu Ulin. | `UShipFleetSubsystem` |
| **Nyai Saraswati — Quartermaster / Agraris dan Jamu** | Pengelola logistik bandar pesisir Jawa dengan pengetahuan pengobatan dan penyimpanan. | Mengurangi penyakit laut; +30% pemulihan stamina/health Hero dan pet; +35% kapasitas cargo Phinisi tanpa menaikkan beban gameplay. | `UShipFleetSubsystem`, `UPetCompanionSubsystem`, character stats |
| **Tuan Gabriel — Garrison Commander** | Mantan perwira tentara bayaran Mestizo/Portugis yang berpihak pada perjuangan maritim Nusantara. Motif dan asal perlu ditulis spesifik, bukan disederhanakan menjadi stereotip. | +50% pertahanan benteng; -20% damage melee yang diterima saat pertempuran dek. | Territory/fort subsystem, `ULanunAttributeSetBase` |
| **Lanun Bintang — Scout / Counter-Espionage** | Pengembara bahari Suku Laut/Bajau dengan navigasi bintang. Konsultasikan istilah dan representasi komunitas. | `Mata Samudra`: radius intelijen dua kali lipat; `Counter-Espionage`: mengurangi atau menggagalkan sabotase wilayah/pasar. | `ALanunAIController`, perception, territory subsystem |

#### Aturan roster dan implementasi Companion

- [ ] Selesaikan riset/naskah asal-usul, penamaan, dialog, motivasi, batas etis, dan representasi untuk seluruh roster.
- [ ] Tiap Companion memiliki spesialisasi, signature quest, perk, hubungan, penugasan, fatigue, loyalitas, serta konsekuensi bila tidak aktif.
- [ ] Tetapkan prioritas buff dan stacking. Bonus yang memengaruhi satu atribut harus melalui aturan modifier bersama, cap, dan UI penjelas.
- [ ] Aktifkan ability hanya jika Companion ditugaskan/berada di party sesuai desain; cegah perk pasif yang tidak masuk akal tetap berlaku dari markas.
- [ ] Uji nilai numerik sebagai baseline, bukan angka final. Cooldown, projectile velocity, income, damage, repair, dan kapasitas harus diuji terhadap progression/economy.
- [ ] Hindari mengunci kesetiaan 100% tanpa konteks cerita; quest dapat memberi jaminan khusus dengan biaya/konsekuensi yang dipahami pemain.

### Rantai faksi

- **Act I — Perang Lada & Selat Melaka:** blokade dan perdagangan Arab/Peranakan; hasil potensial: Singapura sebagai free-trade hub.
- **Act II — Kemutlakan Laut Jawa:** sisa kekuatan Majapahit dan Demak; pemain memilih dukungan, netralitas, atau kepentingan sendiri.
- **Act III — Darah & Rempah di Banda:** konflik Kesultanan dengan Portugis; hasil mengubah kontrol benteng dan rute rempah.

Pilihan faksi harus mengubah akses pelabuhan, harga, dukungan militer, reputasi, dan konflik di wilayah—bukan hanya dialog atau hadiah satu kali.

### Quest mistis

**“Kutukan Dukun Laut & Pusaka Wafak Luwu”**: rumor wabah Doti, investigasi dengan Companion, bahan ritual, penempaan wafak, navigasi gua saat surut, dan pertarungan multi-fase melawan Ki Datu Salisipan. Hadiah rancangan: Jimat Wafak, Babasal kapal, akses ritual Doti, dan rampasan.

Santet, Doti, guna-guna, sesajen, jenglot/banaspati, dan efek supranatural adalah **elemen folklore fiksi**. Tujuan implementasi adalah atmosfer dan gameplay, bukan penggambaran praktik sebagai fakta atau stereotip komunitas.

### Belenggu Suratan Utang — prasyarat Beli Kampung

Quest wajib sebelum panel buyout aktif:

1. Audit akta utang bersama tokoh adat.
2. Pilih penyelesaian: bayar Rupiah/negosiasi bunga; barter komoditas; atau curi bukti pemalsuan untuk membatalkan klaim kreditur.
3. Serahkan pembayaran atau bukti dalam musyawarah.
4. Batalkan akta dan ubah status wilayah menjadi `Free Territory`.
5. Buka panel Beli Kampung dengan harga tanah standar.

Membatalkan utang **tidak** memberikan kampung secara gratis; pembelian hak kelola tetap transaksi terpisah. Jalur pemerasan/sabotase perlu konsekuensi reputasi dan respons faksi. Quest tidak boleh membuat penderitaan warga menjadi sesuatu yang harus sengaja diciptakan pemain.

## 5. Gameplay inti

### Loop permainan

```text
Temukan rumor/kontrak
  -> rencanakan kapal, kru, perbekalan, dan tujuan
  -> berlayar, menghadapi cuaca/ancaman, dan menjelajah
  -> bertarung, berdagang, menyelidiki, atau bernegosiasi
  -> klaim hasil dan hadapi konsekuensi
  -> tingkatkan kapal, kota, hubungan, dan kemampuan
  -> buka konflik/tujuan baru
```

### Kapal dan armada

- Flagship dikendalikan pemain; kapal tempur pengiring memerlukan Companion/perwira dan mendukung formasi `Line of Battle`, `Merchant Escort`, dan `Free Hunt`.
- Kelas target: Pencalang, Lancang, Phinisi, Jung Nusantara, Garay, Sloop, Brigantine, Galleon, dan variasi regional yang tervalidasi.
- Layar, angin, kemudi, draft, kapasitas, kesehatan lambung, komponen tiang/layar/meriam, dan kru memengaruhi performa.
- Kapal kargo yang ditangkap dapat dijadikan kapal tempur, dimasukkan ke rute dagang, dijual, atau didaur ulang menjadi material.
- Kapal dagang pasif memakai simulasi abstrak saat jauh dari pemain; kapal divisualisasikan sebagai actor/proxy saat dekat, lalu state disinkronkan kembali.
- Tidak ada armada tanpa batas tanpa biaya. Kapasitas dermaga, gudang, upah, pemeliharaan, ancaman, dan batas UI menjadi pengendali skala.
- Boarding perlu kondisi grappling yang benar, transisi ke combat dek, objektif, hasil tawanan/rampasan, serta state kapal yang tersinkron.

### Perdagangan, Rupiah, dan logistik

- **Satu mata uang gameplay:** Rupiah; ganti penggunaan `PlayerGold`/`Gold` bertahap dengan ekonomi/domain currency yang konsisten.
- **Maintenance drag:** potong **28% gross revenue** otomatis setelah transaksi penjualan kargo rempah berhasil. Tampilkan gross, potongan, dan net. Jangan kenakan lagi biaya yang sama sebagai biaya transaksi pada penjualan tersebut.
- **Biaya operasi mingguan:** setiap Senin pukul 00:00 waktu game, `UCalendarWorldSubsystem` memproses `CrewWages + (FortCount × 2.500 Rp) + (MineCount × 1.200 Rp)`. Gunakan integer Rupiah dan transaksi atomik; jika kas kurang, jangan buat saldo negatif—tandai kekurangan, hentikan/kurangi produksi pos dan berikan peringatan konsekuensi upah/ransum.
- Maintenance drag 28% dan tagihan mingguan adalah komponen terpisah, dengan basis biaya terdokumentasi supaya upah/repair/ransum tidak terpotong dua kali secara diam-diam.
- Komoditas regional: pala, cengkeh, lada, kapur barus, sutra, candu dalam konteks fiksi; kayu ulin, besi, mesiu, padi, singkong, kina, bawang, cabai, serta ore sebagai kategori tambahan.
- Harga dipengaruhi stok, permintaan, musim, blokade, event, reputasi, biaya transaksi, dan volume perdagangan.
- Dompet, kas markas, inventaris/cargo, stok pasar, pajak, utang, upah dan profit harus menjadi sumber data yang konsisten dan tersimpan.
- Biaya pelabuhan dapat mencakup tambat, bongkar muat, pajak/bea, perbaikan, dan layanan. Faksi, wilayah, popularitas, dan fasilitas memengaruhi biaya.
- Muatan berdampak pada kapasitas, draft, akselerasi, konsumsi, dan risiko cuaca bila hasil prototipe membenarkannya.

### Rute dagang otomatis

- Syarat rancangan: dua pelabuhan dengan fasilitas pasar memadai, kapal kargo tersedia, kapten, komoditas, modal kerja, serta rencana pengawalan.
- Pemain memilih pelabuhan beli/jual, komoditas, volume/modal, kapal, dan escort.
- Siklus perdagangan membayar pemasukan bersih setelah biaya dan risiko; profit masuk ke kas yang tepat.
- Off-screen: simulasi waktu, jarak, musim, biaya, safety, gangguan, dan kerusakan dengan interval hemat CPU.
- On-screen: spawn visual proxy/actor di sekitar pemain, lalu state visual diperbarui dan dikembalikan ke simulasi abstrak saat jauh.
- Incident meliputi badai, pembajakan, blokade, kehilangan sebagian cargo, kerusakan, atau kapal hilang.
- Distress signal memberi pilihan auto-resolve atau intervensi langsung. Travel ke pertempuran harus menjaga state dan biaya waktu.
- Safety Index harus dinormalisasi pada rentang jelas dan dikalibrasi lewat playtest; hindari persentase risiko yang tak konsisten.
- Automated route bukan mesin uang tanpa batas: modal kerja, waktu tempuh, supply/demand, upah, perbaikan, ancaman, pajak, serta kapasitas pelabuhan membatasi profit.
- **Pacing encounter sektor:** target satu encounter tiap sekitar 90 detik atau 1,5–2 mil laut (mana yang tercapai sesuai aturan pemicu tunggal); jangan menjalankan kedua trigger sebagai event terpisah. Encounter perlu cooldown, batas simultan, dan pilihan menghindar/mengabaikan.

### Bagi hasil kapal rampasan

Quick Claim setelah boarding:

1. **Active Fleet:** Companion ditugaskan sebagai kapten; kapal hadir di dunia dan dapat menerima perintah.
2. **Merchant Fleet:** kapal mengikuti rute abstrak; NPC sewaan/crew memenuhi kebutuhan minimum.
3. **Liquidation/scrap:** jual kapal atau bongkar menjadi Rupiah/material.

Batas armada tempur mengikuti jumlah kapten dan kapasitas logistik; armada dagang dapat lebih besar tetapi tetap memakai kapasitas, biaya dan batas simulasi yang terukur.

### Kota pelabuhan dan wilayah

Empat cabang upgrade Level 1–5:

- **Shipyard/Drydock:** repair dan kelas kapal bertingkat hingga flagship/varian legendaris.
- **Coastal Fortifications:** pertahanan, artileri, garnisun, dan ketahanan siege.
- **Marketplace/Warehouse:** kapasitas stok, komoditas langka, dan syarat rute dagang.
- **Tavern/Civil:** perekrutan, kapten langka, layanan sosial, dan mitigasi unrest.

Level tinggi memberi keuntungan yang kuat tetapi punya biaya pembangunan, maintenance, kebutuhan material, dan risiko. Popularitas tinggi dapat mengurangi waktu/biaya melalui bantuan warga; tidak berarti semua upgrade gratis atau kota terkunci pada rating permanen.

Tiga jalur ekspansi:

| Jalur | Cara | Kekuatan | Biaya/risiko |
|---|---|---|---|
| Aliansi | Quest, bantuan, kesepakatan | Fasilitas terjaga, dukungan lokal | Pendapatan dibagi dan kebijakan dibatasi |
| Penaklukan | Blokade, bombardir, assault/siege | Kontrol cepat dan pendapatan besar | Kerusakan, korban, unrest, pemberontakan |
| Beli Kampung | Selesaikan utang, lalu bayar hak kelola | Fasilitas terjaga, Unrest awal 0%, penerimaan awal positif | Memerlukan Rupiah dan penyelesaian prasyarat |

Beli Kampung memberi loyalitas/penerimaan awal yang sangat baik, bukan kekebalan abadi. Popularitas tetap dinamis dan dapat turun karena pajak eksploitatif, kekerasan, atau pengabaian.

### Popularitas, unrest, dan pemerintahan

- Reputasi menggunakan tepat tiga domain gameplay terisolasi, masing-masing dalam rentang -100 hingga +100:
  - **`FactionReputation`:** menentukan hubungan dan agresivitas kapal perang faksi kerajaan/asing saat bertemu di laut. Baseline: Hostile < -30; Neutral -30…+30; Allied > +30.
  - **`TerritoryPopularity`:** menentukan potongan sewa dermaga, risiko unrest/pemberontakan, dan kelancaran buyout. Baseline buyout: minimal +40 dan quest “Belenggu Suratan Utang” harus Completed; popularitas tidak menggantikan prasyarat utang.
  - **`CrewLoyalty`:** menentukan efisiensi tempur perwira/Companion dan risiko mutiny saat krisis ransum/logistik. Jika di bawah -50, lakukan satu pemeriksaan mutiny per pergantian hari.
- Tiga domain memiliki identitas dan penyimpanan terpisah; jangan menggabungkannya menjadi satu meter. `Unrest` dapat menjadi state/risiko hasil pemerintahan yang terpisah, bukan domain reputasi keempat.
- **Sewa dermaga harian:** `BaseRent × (1 − TerritoryPopularity / 200)`. Dengan batas -100…+100, faktor biaya berada di 0,5…1,5; semua hasil dibulatkan secara deterministik ke Rupiah integer.
- **Peluang mutiny harian:** jika `CrewLoyalty < -50`, `clamp(abs(CrewLoyalty + 50) × 1.5%, 0%, 100%)`. Pada -100 hasilnya 75% per hari; hasil pemeriksaan dan penyebab harus terlihat, dan mutiny tidak boleh dieksekusi lebih dari sekali dalam satu hari game.
- Tindakan positif: berbagi hasil, lindungi rute, bangun fasilitas, bantuan bencana, pesta rakyat.
- Tindakan negatif: pajak eksploitatif, upah tertunggak, mengabaikan distress, kekejaman, penjarahan.
- Ambang ekstrem memicu konsekuensi yang bertahap dan dapat dipahami: sabotase, gangguan kerja, blokade, pemberontakan wilayah, atau mutiny kru.
- Penyelesaian militer mengembalikan wilayah dengan dampak reputasi dan populasi; konsiliasi memakai kompensasi, kebijakan pajak, dan negosiasi.
- Companion governor memberi buff sesuai keahlian, tetapi juga memakai slot Companion dan menimbulkan opportunity cost.
- Rentang nilai, perubahan per aksi, ambang dampak, laju pulih, modifier sewa dan risiko krisis perlu dinormalisasi serta diuji; UI harus menjelaskan penyebab perubahan.

### Pertahanan markas dan Companion stationing

- Companion dapat bertugas di Active Fleet, menjadi governor, menjaga markas, atau beristirahat.
- Jumlah slot aktif dibatasi oleh armada dan fasilitas.
- Fatigue/morale pulih ketika Companion beristirahat; waktu dan risiko perjalanan menentukan rotasi.
- Base Defense Score menggabungkan benteng, milisi, kesiapan suplai, dan bonus governor. Formula final perlu menormalkan unit dan diuji.
- Saat markas diserang: peringatan awal, auto-resolve yang menghasilkan korban/kerusakan/rampasan, atau intervensi langsung.
- Companion kuat tidak semestinya selalu menghapus semua kerugian; kesiapan, surprise, ancaman musuh, serta pilihan pemain perlu berpengaruh.

### Pengepungan dan daratan

- Siege dimulai dengan blokade/serangan pantai, diikuti pilihan starve-out, negosiasi, atau assault.
- Supply meter, morale garnisun, jalur logistik, cuaca dan waktu menentukan durasi serta peluang menyerah.
- Assault mencakup landing craft, objektif darat, pertahanan, dan penguasaan pusat komando; kemenangan tidak harus berarti kota utuh.
- Jelajah darat: pantai, hutan, sungai, rawa, gua, tebing, reruntuhan, situs sejarah, treasure dig sites dan komunitas lokal.
- Traversal (lari, panjat, berenang, stealth) perlu prototipe kontrol dasar sebelum Motion Warping dan rangkaian animasi kompleks.
- PCG dapat membantu pengisian bioma; landmark, jalur, gameplay space dan kepadatan tetap perlu authoring/validasi.

### Tambang, perkebunan, dan rantai pasok

- Pos prospeksi/tambang menghasilkan ore; perkebunan menghasilkan pangan dan komoditas sesuai wilayah.
- Companion manager memberi modifier produksi atau safety.
- Hasil harus diangkut melalui rute darat/sungai/laut, diproses, disimpan, dan dijual/dipakai.
- Risiko mencakup cuaca, gangguan, pencurian, keamanan pekerja, reputasi lokal, dan biaya operasi.
- Jangan membuat pemasukan pasif otomatis tanpa batas atau tanpa konsumsi sumber daya.

### Pangan, minuman, dan kuliner

**Ransum dan bahan:** padi, singkong, kina, cabai, bawang, lada, pala, serta daging/hasil panen. Ransum memengaruhi upah, morale, kondisi perjalanan, penyakit, dan risiko mutiny secara terukur.

**Kuliner:**

- Rendang: awet untuk perjalanan jauh; buff ketahanan/damage.
- Ikan Woku: stamina/serangan, dapat meningkatkan haus.
- Nasi Pecel: pemulihan ekonomis.
- Sinonggi/Papeda: ransum dan perlindungan status tertentu.
- Coto Makassar: buff reload/mengurangi fatigue.
- Makanan punya shelf-life berbeda; kualitas gudang dan kondisi pelayaran dapat mengubah spoilage.
- Memasak di kapal/markas memakai bahan, waktu, peralatan, dan Companion koki bila tersedia.
- Hidangan favorit memberi kedekatan Companion; jangan jadikan pemberian satu item cara instan mengunci loyalitas.

**Minuman tradisional dalam game:** Ballo, Cap Tikus, Ciu, Tuak/Saguer, Arak. Efek mencakup buff, drunkenness, distorsi visual, akurasi dan risiko tavern brawl/mutiny. Gunakan dosis, durasi, biaya, toleransi, serta pilihan non-alkohol; efek buruk harus jelas dan tidak mendorong konsumsi berlebihan sebagai strategi dominan.

Pesta kedai dapat meningkatkan popularitas dan membuka rumor dengan biaya Rupiah. Rumor adalah sumber petunjuk yang dapat diverifikasi, bukan jaminan selalu memberi harta.

### Harta karun dan eksplorasi misteri

Loop: rumor/dokumen → fragmen → deciphering → landmark/navigasi → puzzle/boss/ancaman → loot.

Rencana harta:

- Flor de la Mar di Selat Melaka: penyelaman/risiko arus, Rupiah, blueprint.
- Pusaka Mahkota: gua/ruins Jawa, teka-teki, relic Legacy.
- Harta Lanun Sulu: pulau labirin dan armada rival, kapal Garay.
- Batu Permata/Besi Luwu: gua Sulawesi, eksplorasi dan duel, senjata.
- Peti Rempah Kongsi Dagang Barat: infiltrasi benteng Banda, kunci rute dagang.

Companion memberi bantuan parsial: Putri Ratna membaca naskah, Kono melacak jebakan, Daeng Rannu menavigasi wreck, Jarang menghancurkan penghalang. Kemampuan Companion mengurangi hambatan tetapi jangan selalu melewati seluruh permainan.

### Senjata, pakaian, dan combat

Senjata rencana: Keris, Rencong, Kurambik, Badik, Mandau, Sumpit, Parang/Salawaku, Kalis/Kampilan, serta pistol/musket. Setiap senjata membutuhkan moveset, damage type, stamina, jangkauan, kelebihan, kelemahan dan counters; status poison/bleed harus punya aturan serta penangkal.

Stance dan dual slot:

- Badik + pistol: counter dan jarak menengah.
- Parang + Salawaku: defense dan bash.
- Dual Rencong/Kurambik: kecepatan dan stealth.

Upgrade melalui pandai besi memakai Rupiah/material. Racun dan infusi bersifat fiksi game dan membutuhkan bahan/risiko yang jelas.

Armor/pakaian:

- Kawaca, kutang kayu/ijuk, pakaian pesisir, Baju Kurung/Tanjak.
- Cuirass Kongsi Dagang Barat, jubah kapten.
- Pakaian Peranakan dan Hadramaut, chainmail serta perlengkapan terkait.
- Slot kepala/torso/kaki; bobot berdampak pada stamina, berenang, suara, dan kecepatan.
- Set bonus harus memperluas build tanpa mengunci pilihan kosmetik atau membuat satu set wajib.

### Mounted combat dan fauna

- Kuda: traversal, melee, pistol, cavalry charge; serangan mounted memakai animasi upper/lower body dan trace saat jendela serang aktif.
- Kerbau: angkutan berat pada medan lumpur/rawa, dengan batas rute dan biaya perawatan.
- Gajah: unit siege langka dengan kebutuhan suplai dan batas naratif/ekologis.
- Hit collision charge memakai ambang speed, damage cap, cooldown per target, dan validasi ragdoll; hindari impulse tak terbatas atau ragdoll pada actor yang tidak mendukung fisika.
- Arah tebasan ditentukan dari orientasi karakter/kuda dan kamera; crosshair bloom meningkat dengan kecepatan.

Fauna regional berperan sebagai ancaman, sumber yang diatur, atau bagian ekologi:

- Sumatra/Banten: harimau, gajah, tapir.
- Jawa/Bali: badak, macan tutul, kerbau.
- Kalimantan: buaya muara, orangutan, bekantan.
- Sulawesi: anoa, babirusa, tarsius.
- Nusa Tenggara: komodo, kuda.
- Maluku/Papua: kasuari, cenderawasih, kanguru pohon.

Satwa companion target: kakatua/nuri, monyet, harimau, anjing, kucing. Peran: scout, distraction, cargo utility, deck guard, tracking, pest control dan camp morale. Semua kemampuan harus diuji agar tidak meniadakan stealth, ancaman, atau sistem logistik.

**Perlindungan fauna:** harimau Sumatra, anoa, komodo dan Kakatua Raja sebagai satwa endemik/langka berstatus **Protected Wildlife**; bukan komoditas berburu untuk grinding. Interaksi dibatasi pada penjinakan yang sesuai, rescue, Companion, dan perlindungan habitat. Gunakan pelacakan, penyelamatan, konflik satwa-manusia dan material alternatif; jangan memberi insentif perburuan satwa dilindungi.

### Companion, kru, dan satwa

- Roster inti: Daeng Rannu, Panglima Jarang, Putri Ratna, Kono; tambahan: Sultan Baab, Nyai Saraswati, Tuan Gabriel, Lanun Bintang.
- Tiap Companion memiliki spesialisasi laut/darat, hubungan personal, kebutuhan, fatigue, quest loyalitas dan pilihan penugasan.
- Companion dapat memimpin kapal, menjadi governor, menjaga markas, atau beristirahat.
- Kapten NPC prosedural dapat memiliki trait, gaji, loyalitas, dan risiko membelot; sistem Nemesis penuh ditunda sampai loop NPC dasar matang.
- Companion satwa naik level independen, memperoleh skill dan pertumbuhan visual. Kuda/harimau yang dapat ditunggang adalah kategori khusus, bukan fitur otomatis untuk semua pet.
- Sistem tidak boleh membuat Companion eksklusif bersifat “permanen loyal” tanpa menghormati pilihan/konsekuensi cerita.

### Folklore mistis

- Doti: sabotase kapal/morale sebagai efek fiksi.
- Santet: debuff/ilusi/penyakit supernatural dalam aturan game.
- Guna-guna/susuk: mekanik infiltrasi atau manipulasi fiksi.
- Buhul/teluh: kabut, escape, atau fenomena badai.
- Ritual memakai item, waktu, lokasi, dan Companion spesialis; target bisa memiliki ward sehingga ada risiko backfire.
- Wafak Luwu, Doti dan Santet diperlakukan sebagai folklore fiksi bernilai atmosferik; hindari menampilkan ulang ritual sensitif keagamaan dunia nyata sebagai gameplay literal.
- Jimat karakter dan ward kapal mencegah sebagian efek; cleanse memakai quest/lokasi/item.
- Niagara, post-process dan MetaSounds dipicu hanya ketika relevan dan harus menyediakan opsi accessibility untuk camera shake, flash, chromatic aberration, dan audio.

### Nemesis, event dan endgame

#### `URivalNemesisSubsystem` — kapten rival

Subsystem target mengelola memori pertemuan, hubungan, adaptasi taktis dan pilihan naratif. Ini rancangan arsitektur, bukan kelas yang telah diimplementasikan.

State hubungan yang direncanakan:

- `HostileNemesis`: aktif memburu pemain.
- `Vendetta`: menggalang sekutu/armada dan menyiapkan balas dendam.
- `NeutralRival`: pesaing dagang atau lanun independen.
- `BloodBoundAlly`: sekutu yang dapat dipanggil dengan biaya/kondisi.
- `SparedOwesLife`: pernah diampuni dan memiliki utang nyawa.
- `Betrayer`: status/peristiwa terpisah untuk sekutu yang mengkhianati perjanjian; jangan gabungkan dengan status “diampuni”.

Data rival minimal perlu memuat ID kapten/faksi, state hubungan, lokasi terakhir, hitungan kalah/lolos, pilihan pemain (ampuni, rampas dan bebaskan, ajak sekutu, eksekusi), perjanjian, dan waktu encounter. Jangan menyimpan hanya bool permanen seperti `bPlayerUsedRamming`: simpan beberapa encounter terakhir, frekuensi/recency gaya pemain dan konteksnya agar adaptasi tidak terkunci oleh satu aksi lama.

Adaptasi dan counterplay:

- Jika pemain sering memakai ramming, rival dapat memasang penguatan/paku kayu besi yang **mengurangi sebagian** bonus ramming—bukan membatalkannya sepenuhnya.
- Jika pemain terlalu mengandalkan broadside jarak jauh, rival dapat memilih kiting cepat; taktik ini tetap punya counter yang terbaca melalui escort, posisi, cuaca atau pendekatan lain.
- UI, dialog, atau intel Companion memberi petunjuk tentang perubahan strategi rival.
- Setelah kapal rival dikalahkan, pilihan dapat berupa eksekusi, rampas kargo lalu bebaskan, atau tawarkan aliansi. Setiap opsi mengubah reputasi, hubungan faksi dan kemungkinan encounter berikutnya.
- Sekutu rival tidak gratis atau tanpa syarat: pemanggilan memakai cooldown/konteks dan mempertimbangkan kesetiaan serta keadaan faksi.

#### `UNusantaraWorldStateSubsystem` — kampanye, faksi dan event

Subsystem target mengelola state kampanye lintas level dan interval waktu, bukan simulasi setiap frame. Actor/kapal visual aktif berada pada World/level; state strategis yang persisten berada di subsistem kampanye dan disimpan.

Node pelabuhan dapat memiliki `ControlFaction`, `MilitancyIndex`, `WealthScore`, supply, market index, dan status blokade. Faksi dapat berdagang, mempertahankan, menyerang, memberontak, membentuk koalisi atau mengubah kontrol. Indeks dan formula harus dinormalisasi serta dibatasi agar perubahan dapat dipahami dan diuji.

Event rancangan:

| Event | Efek gameplay | Konsekuensi/peluang |
|---|---|---|
| Badai muson ekstrem | Gelombang/cuaca meningkatkan risiko rute | Perubahan harga rempah bersifat sementara, lokal dan memiliki batas; bukan lonjakan universal otomatis 200%. |
| Wabah pesisir | Kekurangan awak, gangguan stamina/supply | Misi bantuan pangan/Jamu dan hambatan logistik. |
| Blokade bandar | Akses pelabuhan terhalang | Menyusup dengan perahu kecil, menegosiasikan akses, atau menghancurkan armada blokade. |
| Rumor konvoi bernilai tinggi | Intel lokasi/waktu konvoi | Misi berisiko dengan Rupiah dan blueprint sebagai hasil potensial; rumor harus konsisten dengan dunia. |
| Distress call | Kapal sekutu/rival meminta pertolongan | Mengubah reputasi, hubungan Nemesis, kerusakan, cargo dan hasil konflik. |
| Pemberontakan warga | Kontrol pelabuhan terancam | Membela penguasa, mendukung warga, menengahi, atau mengejar jalur kepemilikan yang sah. |

#### Quest prosedural dan validasi

- Maksimal **3 quest prosedural aktif** dalam campaign single-player.
- Quest hanya ditawarkan saat wilayah, waktu, keadaan dunia dan hubungan relevan.
- Setiap template wajib memiliki prasyarat, tujuan yang dapat diselesaikan, kondisi gagal, reward, konsekuensi, dan penyimpanan state.
- Hindari duplikasi, pengulangan beruntun, serta quest fetch tanpa konteks atau variasi.
- Keberhasilan/kegagalan dapat mengubah `Faction Influence` atau `Market Price Index`, tetapi dampak harus punya cap, decay/durasi dan feedback UI.
- Procedural Validation Engine adalah tanggung jawab desain; tidak mengandaikan adanya generator quest yang sudah ada.

#### Legacy Score, token dan pensiun

Legacy Score mengukur dampak sepanjang kampanye, bukan kekayaan akhir atau jumlah kampung. Setiap kategori terlebih dahulu dinormalisasi ke **0–100** berdasarkan target/cap yang ditetapkan, lalu dihitung:

```text
Legacy Score =
  0.30 × Campaign Milestones
+ 0.20 × Regional Stewardship
+ 0.15 × Diplomacy & Faction Outcomes
+ 0.15 × Exploration & Quest Achievements
+ 0.10 × Companion Bonds
+ 0.10 × Trade & Logistics
```

- `Campaign Milestones`: babak utama dan pencapaian unik.
- `Regional Stewardship`: stabilitas, kesejahteraan dan fasilitas; bukan jumlah kampung semata.
- `Diplomacy & Faction Outcomes`: hasil aliansi, perdamaian dan keputusan faksi.
- `Exploration & Quest Achievements`: lokasi/treasure/quest unik; aktivitas berulang tidak boleh menggandakan skor tanpa batas.
- `Companion Bonds`: quest kesetiaan dan hubungan yang dipelihara.
- `Trade & Logistics`: kinerja rute dan pengelolaan; bukan saldo Rupiah akhir.

Baseline konversi: **1 Legacy Token per 5 poin Legacy Score**, maksimum **20 token**. Tampilkan kategori, perolehan, batas dan token sebelum pemain mengonfirmasi pensiun. Token membuka mentorship traits, misalnya Sensitivitas Angin Bajau atau Keturunan Pandai Besi Ulin. Nilai ini perlu diuji lewat playtest; tidak ada bonus token tersembunyi dari penimbunan Rupiah.

#### Pewarisan generasi (baseline desain)

| Kategori | Warisan | Aturan |
|---|---|---|
| Markas dan benteng | 100% yang masih dikuasai | Wilayah dapat diserang/diperebutkan; kepemilikan tidak berarti keadaan selalu aman. |
| Armada | 1 flagship + hingga 30% kapal escort | Bulatkan ke bawah, minimum 0 escort; tetapkan batas maksimum sesuai performa. Kapal yang tidak diwariskan dapat dijual/dikonversi menjadi modal sesuai UI transisi. |
| Kargo dan gudang | 40% material; 100% blueprint | Pembulatan material ditampilkan; blueprint yang sudah dibuka tetap tersedia. |
| Kas Rupiah | 25% | Sisa Rupiah tidak otomatis dikonversi ke Legacy Token. Token hanya berasal dari Legacy Score. |
| Perwira/Companion | 100% manfaat gameplay dipertahankan melalui transisi karakter atau institusi | Individu tidak otomatis hidup/melewati lompatan waktu. Perk, akses, dan blueprint yang sudah diperoleh tetap tersedia dalam bentuk penerus, mentor, atau institusi. |
| Karakter lama | Mentor, penerus gelar, atau pendiri institusi | Nasib ditentukan per karakter dan ditampilkan sebelum konfirmasi. |

##### Tiga jalur transisi Companion

Lompatan waktu antargenerasi baseline adalah **20–30 tahun**. Perwira fisik tidak dipertahankan tanpa perubahan: tiap Companion memiliki jalur eksplisit, sementara bonus/ability yang telah diperoleh pemain tetap tersedia dalam bentuk yang masuk akal.

| Companion | Jalur transisi | Hasil naratif | Manfaat yang tetap tersedia untuk generasi penerus |
|---|---|---|---|
| **Daeng Rannu** | Senior Mentor (pensiun) | Menetap di benteng utama sebagai penasihat perang. | Aura mentor benteng: +15% Rudder Torque global untuk armada penerus. |
| **Nyai Saraswati** | Senior Mentor (pensiun) | Menjadi matriarch pengobatan dan logistik markas. | Membuka Toko Jamu/Farmasi markas dan pencegahan scurvy kapal. |
| **Tuan Gabriel** | Senior Mentor (pensiun) | Memimpin garnisun dan melatih prajurit benteng. | +50% pertahanan benteng dan pelatihan awal prajurit boarding. |
| **Panglima Jarang** | Penerus Gelar | Murid/penerus muda mengambil gelar dan komando. | Penerus ber-avatar baru; mewarisi Salisipan Broadside dan Aura Artileri. |
| **Kono** | Penerus Gelar | Pejuang muda meneruskan gelar Penjaga Rimba melalui tradisi dalam cerita. | Pos tambang emas Papua dan Serangan Sumpitan Racun tetap tersedia. |
| **Lanun Bintang** | Penerus Gelar | Navigator muda terbaik meneruskan jabatan Master Scout. | Mata Samudra dan Counter-Espionage tetap tersedia pada penerus. |
| **Putri Ratna** | Legasi Institusi (Kongsi Dagang Barat) | Dewan agen meneruskan jaringan intelijen Melaka. | Akses Pasar Gelap dan Margin Smuggler tetap aktif melalui institusi. |
| **Sultan Baab** | Legasi Institusi (Guild) | Serikat pandai besi dan galangan Ulin meneruskan karyanya. | Blueprint Ulin dan Auto Hull Repair tetap terbuka melalui guild. |

Penerus gelar adalah **karakter baru**, bukan individu lama yang disamarkan. Mereka mewarisi kemampuan/pengetahuan yang relevan dengan gelar, tetapi statistik pribadi, hubungan, suara, rupa, dan cerita mereka dapat berbeda. Mentor dan institusi menjaga manfaat yang tidak cocok diwariskan secara biologis atau personal.

##### Data transisi dan penyimpanan

Rancangan perubahan data simpan dari `FOfficerSaveState` menjadi `FOfficerLegacyState`:

```cpp
UENUM(BlueprintType)
enum class ECompanionLegacyStatus : uint8
{
    ActiveOnBoard,
    RetiredBaseMentor,
    InstitutionalGuild
};

USTRUCT(BlueprintType)
struct FOfficerLegacyState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName OfficerID = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    ECompanionLegacyStatus LegacyStatus = ECompanionLegacyStatus::ActiveOnBoard;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 GenerationIndex = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName SuccessorOfficerID = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float InheritedLoyaltyBonus = 20.0f;
};
```

Enum tersebut adalah state persistensi, bukan keseluruhan silsilah. Penerus gelar membutuhkan ID asset penerus yang tervalidasi; mentor/institusi membutuhkan status dan reward yang tidak bergantung pada actor lama. `InheritedLoyaltyBonus` adalah nilai baseline yang perlu dibatasi/dikaji per Companion; jangan menambahkannya berulang setiap kali Legacy dijalankan. Jika save lama sudah berisi `FOfficerSaveState`, migrasikan datanya dengan versi schema alih-alih menghapus/menimpa save.

##### Legacy Lock Confirmation UI

Sebelum aksi **“Wariskan Legasi (Legacy Succession)”**, panel UMG menampilkan:

- Perwira penerus gelar, avatar/ID penerus, serta ability dan perk yang diteruskan.
- Mentor yang pensiun dan aura/fasilitas yang akan mereka berikan.
- Institusi (Kongsi Dagang Barat/Guild), akses dan blueprint yang tetap aktif.
- Perubahan state Companion satwa dan bonusnya selama lompatan waktu; aturan transisi satwa harus menghormati usia/spesies dan tidak menghilangkan kemampuan yang sudah dibuka.
- Aset yang diwariskan/di-reset, Legacy Score/Token, hubungan wilayah, serta semua efek yang tidak diteruskan.
- Tombol konfirmasi yang eksplisit dan opsi batal; jangan menimpa satu-satunya save generasi lama secara diam-diam.

**Invarian desain:** tidak ada perk, ability, akses, atau bonus gameplay yang telah diperoleh pemain hilang hanya karena pergantian generasi. Bentuk kepemilikan dapat berubah menjadi mentor, successor, atau institusi; perubahan bentuk dan batas benefit dijelaskan sebelum Legacy Lock.

#### Endgame sandbox

- Faksi yang kalah dalam cerita dapat bertahan sebagai sel, koalisi atau ekspedisi balasan; hindari menghapus pencapaian utama dengan respawn tanpa konteks.
- Konvoi mitos dan encounter supernatural menjadi event langka dengan telegraph, counterplay dan opsi accessibility.
- Pemain menjaga rute Banda–Melaka–Ternate dari embargo, sabotase, fluktuasi pasar dan penyusupan.
- Tekanan dunia harus menantang tapi berirama: masa pemulihan setelah event besar, batas simultan event, serta pilihan untuk mengabaikan konflik dengan konsekuensi yang dipahami.

## 6. Sistem progresi final: Level 1–100

Semua entitas memakai level maksimum 100 dengan progres independen: Hero, Companion/perwira, satwa, dan markas. Tidak berarti mereka memakai sumber XP atau kurva hadiah yang sama persis.

### Kurva XP Hero

XP untuk naik **dari level L ke L+1**, untuk `L = 1…99`:

```text
XPRequired(L) = floor(500 + 50 × L + 9 × L^1.8)
```

Fungsi target `CalculateRequiredXPForLevel(int32 CurrentLevel)` mengembalikan 0 untuk level di bawah 1 atau `CurrentLevel >= 100`, lalu menghitung formula dengan `double` sebelum floor dan mengembalikan integer. Nilai threshold ini aman dalam rentang `int32` pada Level 1–99. Implementasi dan tes belum ditemukan saat audit.

| Level saat ini | XP ke level berikut | Total XP untuk mencapai level saat ini |
|---:|---:|---:|
| 1 | 559 | 0 |
| 10 | 1.567 | 8.500 |
| 25 | 4.704 | 51.913 |
| 50 | 13.289 | 264.351 |
| 75 | 25.597 | 736.901 |
| 99 | 40.637 | 1.518.089 |
| 100 (cap) | 0 | 1.558.726 |

**Koreksi tabel kumulatif terdahulu:** angka kumulatif harus didefinisikan sebagai jumlah threshold `L=1` sampai `L=CurrentLevel-1`. Karena itu 1.558.726 XP adalah total untuk mencapai Level 100, bukan total untuk mencapai Level 99. Gunakan tabel ini sebagai nilai uji golden untuk implementasi C++.

Hadiah XP awal untuk playtest sebagai persentase XP level berikutnya:

- Aktivitas eksplorasi/pembukaan peta: sekitar 4%.
- Side quest: sekitar 8%.
- Main quest: sekitar 18%.
- Boss/siege besar: sekitar 30%.

Contoh pada Level 20 (3.477 XP ke level berikut), dengan reward dihitung `floor(XPRequired × persentase)`:

| Aktivitas | Persentase | XP reward Level 20 |
|---|---:|---:|
| Eksplorasi/pembukaan peta | 4% | 139 |
| Side quest | 8% | 278 |
| Main quest | 18% | 625 |
| Boss/siege | 30% | 1.043 |

Nilai harus memperhitungkan repeatability, kesulitan dan durasi, serta mencegah farming transaksi tanpa risiko. Contoh eksternal yang membulatkan 4% menjadi 140 adalah pendekatan; implementasi menggunakan floor agar aturan deterministik.

### Hero

- **Poin awal:** Level 1 dimulai dengan 0 Attribute Points dan 0 Skill Tree Points.
- **Level-up:** setiap kenaikan level dari Level 1 menuju Level 2 sampai Level 100 memberi +3 Attribute Points dan +1 Skill Tree Point, total maksimum 297 AP dan 99 SP dari level-up.
- Pilar atribut: Strength, Agility, Leadership, Mysticism.
- **Biaya unlock:** Tier 1 = 1 SP; Tier 2 = 2 SP; Tier 3 = 3 SP; Tier 4/Ultimate = 5 SP. Tier skill mengikuti prasyarat atribut/level pada matriks. Saat syarat umum dan skill-specific berbeda, gunakan syarat yang lebih ketat; misalnya Mahkota Samudra tetap memerlukan Leadership 60 dan Level 100.
- **Respec:** tersedia di Bandar Somba Opu melalui NPC Dukun Laut/Rumah Adat, biaya `HeroLevel × 1.000 Rp`, cooldown satu minggu kalender game. Reset mengembalikan seluruh AP dan SP yang telah dialokasikan/dibelanjakan; level dan XP tidak berubah. Konfirmasi UI menampilkan biaya, cooldown dan reset yang dilakukan.
- **Atribut efektif:** raw attribute maksimal 100. Nilai efektif memakai diminishing returns: raw 1–50 dihitung 1,0x; 51–80 dihitung 0,5x; 81–100 dihitung 0,25x. Dengan formula marginal `Effective = min(raw,50) + 0.5×clamp(raw−50,0,30) + 0.25×clamp(raw−80,0,20)`, hard cap efektif adalah 70. Batas ini berlaku pada bonus statistik berbasis atribut, bukan mengganti nilai atribut yang dipakai sebagai syarat unlock.
- Milestone: Level 10 dual weapon; 25 minuman Tier 2; 50 Bhayangkara Spirit dan slot jimat tambahan; 75 ultimate stance; 100 gelar Imperator Samudra/Legacy Supreme dan bonus Rupiah pasif.
- “Kekebalan total” di Level 100 hanya berlaku pada efek yang dapat diseimbangkan, bukan kebal terhadap seluruh damage/konsekuensi.

### Skill Tree Hero dan integrasi GAS

Empat cabang skill menghubungkan atribut Hero ke active ability on-foot/sailing dan passive gameplay effects. Persyaratan level/atribut adalah prasyarat unlock; Companion dapat memperkuat ability tertentu hanya jika Companion tersebut hadir/ditugaskan sesuai aturan gameplay.

| Cabang | Tier | Skill | Tipe | Prasyarat | Efek rancangan |
|---|---|---|---|---|---|
| Keperkasaan | T1 | Tebas Keris/Badik | Active, on-foot | Level 1 | Combo tiga sabetan; pulihkan stamina 10% jika mengenai target. |
| Keperkasaan | T2 | Hantaman Salawaku | Active, on-foot | Strength 15 | Shield bash yang men-stagger musuh selama 1,5 detik. |
| Keperkasaan | T3 | Tangkas Salawaku | Active, on-foot | Strength 30 | Jendela parry 0,6 detik; serangan yang berhasil ditangkis membatalkan damage dan membuka counter. |
| Keperkasaan | T4 | Bhayangkara Strike | Ultimate active | Strength 50, Level 50 | Serangan berat unblockable; damage zirah 2x, bukan pengabaian seluruh pertahanan/aturan boss. |
| Kelincahan | T1 | Elak Gelombang | Active, mobility | Level 1 | Dodge roll dengan i-frame 0,35 detik. |
| Kelincahan | T2 | Jejak Angin | Passive | Agility 15 | Sprint speed +15%; konsumsi stamina memanjat/berenang -25%. |
| Kelincahan | T3 | Dual Stance Switch | Mode switch | Agility 30, Level 10 | Berganti Single Keris (cepat) dan Badik + Pistol (jarak menengah). |
| Kelincahan | T4 | Langkah Bayangan | Active, stealth | Agility 50, Level 75 | Gerak senyap 6 detik; serangan pertama dari belakang mendapat critical damage +150%. |
| Kepemimpinan | T1 | Kemudi Muson | Passive, sailing | Level 1 | Rudder torque +15%. |
| Kepemimpinan | T2 | Tactical Ramming | Active, sailing | Leadership 20 | Dengan Daeng Rannu: thrust +50% selama 4 detik, ramming damage +60%, damage balasan -40%. |
| Kepemimpinan | T3 | Salisipan Broadside | Active, sailing | Leadership 35 | Dengan Panglima Jarang: reload -50% selama 12 detik dan efek ignite dengan batas durasi/stack. |
| Kepemimpinan | T4 | Mahkota Samudra | Passive, sovereign | Leadership 60, Level 100 | Escort HP +20% dan hasil Rupiah perdagangan +15%, dengan aturan tidak menumpuk tanpa batas. |
| Kebatinan | T1 | Sumpitan Racun | Active, ranged | Level 5 | Dengan Kono: racun DoT 8 detik dan slow 40%; status/boss mendapat resistansi yang terlihat. |
| Kebatinan | T2 | Ramuan Jamu | Passive, utility | Mysticism 15 | Dengan Nyai Saraswati: durasi buff kuliner 2x; proteksi scurvy mengikuti aturan kru/logistik, bukan kekebalan global tanpa syarat. |
| Kebatinan | T3 | Wafak Samudra | Passive, ward | Mysticism 35, Level 50 | Membuka slot Jimat Wafak kedua; status resistance +30% terhadap Doti/racun. Regenerasi lambung 0,5%/detik perlu dibatasi pada kondisi/kapal yang jelas dan diuji balance. |
| Kebatinan | T4 | Perisai Luwu | Ultimate passive | Mysticism 55, Level 75 | Serap 50% damage supernatural dan kurangi efek cuaca buruk pada karakter; tidak meniadakan bahaya lingkungan kapal. |

#### Kontrak implementasi dan batas C++ saat ini

- `ULanunAttributeSetBase` yang ada hanya memiliki Health, MaxHealth, Stamina, MaxStamina, ArmorWeight, dan SwimmingBuoyancy. Belum ada stat Strength/Agility/Leadership/Mysticism, level, XP, attribute points, skill points, tree unlock, atau pemberian AbilitySpec.
- AttributeSet juga dibuat pada karakter AI. Karena itu `UnallocatedAttributePoints` dan `UnallocatedSkillPoints` tidak boleh ditambahkan sebagai state progresi pemain di AttributeSet tanpa memisahkan state Hero dari AI. Simpan progresi/poin di sistem progresi Hero yang persisten, validasi alokasi pada gameplay authority lokal, lalu terapkan atribut/GameplayEffect ke ASC secara konsisten.
- Jangan izinkan poin negatif, pembelian unlock yang tidak cukup poin, atribut raw di atas 100, atau unlock melewati syarat level/atribut. Level 1 tidak menerima poin level-up.
- `ULanunGameplayAbilityBase` memakai `ELanunAbilityInputID`, bukan `AbilityInputTag`, dan saat ini tidak mengurangi `StaminaCost` dengan sendirinya. Skill baru harus disesuaikan dengan jalur input yang ada atau migrasi input ditangani konsisten; biaya stamina perlu GameplayEffect/commit yang benar-benar tervalidasi.
- Tangkas Salawaku belum boleh dianggap selesai hanya dengan memasang `State.Combat.Parrying` dan timer. Execution damage harus mengenali jendela parry, membatalkan damage dengan benar, mengirim `Event.Combat.ParrySuccess`, dan memicu counter sekali saja; end/cancel ability wajib menghapus tag/timer. Montage/animasi dan konfigurasi GameplayEffect disiapkan di aset setelah kode dasar tersedia.
- Tag taxonomy yang dibutuhkan telah didaftarkan pada `DefaultGameplayTags.ini`. Tag saja tidak memberikan ability, tidak melakukan bind input, dan tidak membuktikan efek gameplay telah bekerja.
- Nilai persentase, durasi, i-frame, DoT, multiplier dan efek armada adalah baseline balance untuk playtest; seluruh buff harus memiliki cap, aturan stacking, validasi gameplay authority lokal, dan UI feedback. Jangan menambah replication/network dependency selama scope offline-first.

### Companion/perwira

- Naik melalui tugas dan kontribusi peran.
- Perk tiap beberapa level; contoh tier awal membuka command, pertengahan aura/loyalty utility, tinggi signature skill/weapon.
- Loyalitas tidak otomatis terkunci 100%; quest dapat memberi perlindungan/komitmen khusus, tetapi keputusan pemain dan kondisi cerita tetap penting.

### Satwa

- Level 1–100; tahap Anak 1–25, Remaja 26–60, Dewasa 61–100.
- XP dari feeding, eksplorasi, pertempuran dan penemuan, dengan reward sesuai peran dan risiko.
- Skill tree/pasif dibatasi slot atau biaya agar pemain memilih peran, bukan membuka semua bonus sekaligus.
- Growth stage harus memetakan spesies dengan benar; hanya satwa yang layak menjadi mount yang membuka tunggang.

### Markas

- Level 1–25: Sarang Pesisir.
- Level 26–50: Benteng Pesisir.
- Level 51–75: Kota Pelabuhan Merdeka.
- Level 76–100: Sovereign Island Realm.
- Base XP berasal dari pembangunan, supply chain, misi, dan pengelolaan; bukan sekadar menghabiskan uang.
- Slot kapal, gudang dan Companion bertambah bertahap, dengan biaya operasi dan batas performa.

## 7. Konten ekonomi dan identitas regional

### Komoditas dan peran

| Wilayah | Komoditas/rencana peran |
|---|---|
| Jawa/Bali | Padi, bawang, kuliner, pasar utama |
| Maluku/Banda | Pala, cengkeh |
| Sumatra/Banten | Lada, rendang, jaringan barat |
| Priangan/dataran tinggi | Kina/obat dalam setting game |
| Sulawesi/Nusa Tenggara | Cabai, singkong, Ballo, Besi Luwu |
| Papua/Timor | Singkong, kayu manis, tambang dan rute frontier |

### Demografi dan reputasi

- Barat: Melayu, Aceh, Minang, Sunda, Peranakan, Hadramaut; perdagangan, pinjaman, sutra/keramik/lada.
- Tengah: Jawa, Madura, Dayak, Bugis, Makassar, Sulu; pelaut, komoditas lokal, galangan dan faksi kerajaan.
- Timur: Ternate/Tidore, Banda, Timor, Papua, Portugis; rempah, benteng dan artileri.
- Kelompok sosial/faksi memiliki reputasi berbeda. Hindari menggabungkan etnis dengan sifat moral tetap; karakter, tindakan dan kepentingan harus beragam.
- Pedagang Arab/Peranakan dapat memberi investasi, pasar gelap, akses impor, rumor, atau kontrak; Portugis dapat menjadi lawan atau mitra.

## 8. Platform, layanan, dan kebutuhan teknis

### PC

- Satu-satunya platform aktif untuk vertical slice dan rilis awal: Windows Win64, DX12/SM6, target workstation RTX 2000 Ada.
- Keyboard/mouse dan gamepad melalui Enhanced Input.
- Profil kualitas, rendering, streaming, dan performa perlu dibangun serta dites. Shipping Build harus dipaketkan dan diuji sebelum membuka kembali platform lain.

### Android/iOS

- **FROZEN:** tidak termasuk vertical slice maupun scope rilis yang sedang dikerjakan.
- Evaluasi porting baru dibuka setelah Shipping Win64 terverifikasi stabil dan siap dimainkan; kebutuhan renderer, touch UI, thermal budget dan packaging dianalisis saat scope dibuka kembali.

### Pengaturan aksesibilitas

| Opsi | Rentang/default | Efek target |
|---|---|---|
| Camera Shake | Slider 0–100%, default 100% | Mengatur intensitas `UCameraShakeBase` untuk ledakan meriam dan badai. |
| Motion Blur | Enabled/Disabled | Mengatur motion blur pada post-process; opsi Disabled menonaktifkannya. |
| Reduced Flash | Normal/Reduced Flash | Meredupkan kilat badai dan muzzle flash tanpa menghilangkan telegraph gameplay. |
| HUD Scaling | 80–120% | Mengubah skala UI/HUD dengan layout yang tetap terbaca dan tidak terpotong. |

Preferensi disimpan lokal. Nilai aktual pada post-process, camera shake, flash, dan DPI harus dites dalam build; jangan hanya mengubah CVar tanpa memastikan semua sumber efek mengikuti opsi.

### Save dan akun

- **Offline-first lokal Win64:** data campaign disimpan oleh `ULanunSaveGameSubsystem` memakai save lokal Unreal, dengan schema version, migrasi, validasi dan recovery.
- **Protokol dual-file:** serialize ke staging slot/file sementara; load kembali dan validasi objek/schema/checksum sebelum menyentuh primary. Bila valid, pindahkan primary valid menjadi `.bak`, lalu commit staging menjadi primary. Jika commit gagal, pertahankan atau pulihkan primary/backup terdahulu dan laporkan error UI; jangan laporkan sukses sebelum primary baru dapat dibaca.
- Backup harus disimpan sebagai salinan bytes yang valid untuk format save Unreal. Gunakan API slot dan atomic replace/rename yang didukung Windows; jangan mengasumsikan `SaveGameToSlot("SaveSlot.sav.tmp")` menghasilkan path persis seperti suffix tersebut atau menyalin `.bak` lalu memuatnya tanpa memulihkan nama slot yang benar.
- **Fallback load:** coba primary; jika tidak dapat dibaca/valid, coba backup; hanya setelah backup berhasil dibaca, pulihkan primary. Jika keduanya invalid, pertahankan file untuk diagnosis dan laporkan kegagalan—jangan memulai save baru seolah-olah load berhasil.
- **Schema migration:** tambahkan `SaveSchemaVersion` dan migrator eksplisit (termasuk `FOfficerSaveState` → `FOfficerLegacyState`); migrasi di memori lalu simpan ke staging/version baru dan verifikasi sebelum mengganti file lama.
- **Lokasi target:** `%LOCALAPPDATA%/Tides_Of_Nusantara/Saved/SaveGames/`. Verifikasi bahwa implementasi memakai lokasi per-user yang aman sesuai platform Unreal dan jangan menulis langsung ke direktori instalasi.
- **Kompatibilitas slot:** implementasi saat ini memakai slot `LanunSaveSlot_0`; migrasi ke nama/format `SaveSlot` harus tetap membaca save yang sudah ada dan hanya menghapus/mengganti file lama setelah save baru berhasil diverifikasi.
- Save perlu memuat dompet Rupiah, inventory, kapal, rute, quest, Companion, tiga domain reputasi, kota, progresi, event dunia dan Legacy.
- **Online dan cross-save DEFERRED:** EOS/Firebase/FastAPI tidak diperlukan untuk vertical slice; jangan menambah akun/backend/network dependency sebelum keputusan scope baru.

### Toolchain

- UE5 C++/Blueprint; validasi versi engine dan kompatibilitas plugin.
- Windows development dengan Visual Studio/MSVC.
- Android/iOS toolchain bukan prasyarat aktif selama platform berstatus FROZEN; hanya ditinjau jika scope mobile dibuka kembali.

### Aset dan plugin

- Core module dependency C++ wajib mencakup GAS (`GameplayAbilities`), Enhanced Input, Water (`Water`), StateTree (`StateTreeModule`, `GameplayStateTreeModule`), Mass (`MassEntity`, `MassCrowd`, `MassActors`) serta UI (`UMG`, `Slate`, `SlateCore`) sesuai pemakaian source. Bedakan **nama plugin** yang di-enable di `.uproject` dari **nama module dependency** di `Build.cs`; plugin `MassGameplay` tidak boleh dimasukkan sebagai module dependency. Nama plugin untuk fitur WaterAdvanced/StateTree/Mass harus diverifikasi terhadap descriptor UE 5.8.4, bukan disimpulkan dari nama module.
- Plugin pihak ketiga berstatus **DEFERRED** sampai vertical slice lulus kualifikasi; hindari penambahan dependency pihak ketiga selama tahap stabilisasi.
- UDS/Oceanology/Waterline tetap kandidat opsional, bukan dependensi yang sudah diputuskan. Jika kelak dievaluasi, periksa kualitas, performa, kompatibilitas, dukungan dan lisensi.
- Aset kapal, budaya, senjata, pakaian, musik dan audio perlu provenance/lisensi yang jelas.
- SQLite atau sistem serialisasi lokal belum diputuskan; gunakan format save UE yang teruji sebelum menambah database terpisah.

## 9. Ide tambahan yang disetujui

1. **Konsekuensi ekspansi berbeda:** aliansi, buyout dan conquest punya biaya/manfaat jangka panjang yang nyata.
2. **Konflik nilai protagonis:** karakter yang kehilangan rumah menghadapi pilihan untuk memutus atau mengulang pola eksploitasi.
3. **Rival bernarasi:** kapten yang lolos dapat kembali, berdamai, meminta bantuan, atau mengubah strategi berdasarkan peristiwa sebelumnya.
4. **Perdagangan sebagai cerita:** kontrak, blokade, perubahan musim, barang rusak, suap dan permintaan darurat menjadi gameplay, bukan hanya arbitrase angka.
5. **Objektif naval combat bervariasi:** lumpuhkan tiang, lindungi kapal sipil, rebut perwira, bertahan dari badai, atau kabur dari armada lebih kuat.
6. **Ekonomi endgame terkendali:** upah, repair, risiko, supply, pajak, kapasitas dan waktu menjaga income pasif tetap bermakna.
7. **Sejarah alternatif dinyatakan eksplisit:** khususnya relasi Majapahit dan setting tahun 1521.
8. **Perlindungan fauna langka:** Protected Wildlife assets tidak dapat diburu untuk grinding; rescue, Companion yang sesuai dan perlindungan habitat menjadi interaksi.
9. **Aksesibilitas efek:** pengaturan Camera Shake Toggle, Motion Blur Reduction, Flash/Lightning Attenuation, skala HUD, distorsi, dan audio intens.
10. **Folklore sebagai fiksi yang menghormati konteks:** riset, konsultasi budaya bila memungkinkan, dan representasi yang tidak menggeneralisasi masyarakat nyata.

## 10. Urutan implementasi (TODO)

### Tahap 0 — Stabilkan proyek

- [ ] Verifikasi source build UE 5.8.4 terdaftar dan terhubung ke GUID lokal `EngineAssociation` pada `.uproject`; asosiasi target UE 5.8.4 sudah diputuskan tetapi pemetaan GUID/build belum terverifikasi.
- [ ] Compile editor/game Win64 pada target RTX 2000 Ada dan validasi DX12/SM6, Lumen/HWRT, VSM, TSR, Substrate, volumetric fog serta shader pipeline cache.
- [ ] Verifikasi setiap nama plugin pada `.uproject` terhadap descriptor UE 5.8.4 dan setiap module dependency pada `Build.cs` terhadap modul source; jalankan editor startup, cook serta packaging Win64. Jangan gunakan nama module sebagai nama plugin tanpa verifikasi.
- [ ] Siapkan konfigurasi Development untuk testing dan Shipping untuk distribusi; pastikan console dan debug symbols tidak disertakan pada hasil Shipping.
- [x] Compile modul setelah getter `GetBoardState()`/`GetShipMovementComponent()` ditambahkan; **selesai — kompilasi UE 5.8.4 berhasil**; verifikasi error tersisa dan tambah tes/smoke test dasar.
- [ ] Siapkan peta startup valid, game mode, pawn, input mapping karakter/kapal, serta jalur packaging PC.
- [ ] Verifikasi startup, possession, Enhanced Input, dan perpindahan konteks on-foot/sailing dalam PIE.
- [ ] Uji KPI sektor pada RTX 2000 Ada: 60 FPS Editor 1080p High/TSR Quality, game VRAM ≤7,5 GB, pesisir-ke-karang 2–4 menit, laut lepas 5–8 menit; catat peak/1% low/hitch. Jangan jadikan Ollama 14B sebagai persyaratan atau asumsikan dapat berbagi VRAM.
- [ ] Buat prototipe sektor 10 × 10 km Selat Makassar/Spermonde; ukur local/ocean travel scale, transisi sektor, streaming, memori/VRAM dan kepadatan gameplay.
- [ ] Bangun dua pelabuhan slice: Bandar Somba Opu (perdagangan/quest/pasar gelap) dan Teluk Tersembunyi Spermonde (markas/kapal/Beli Kampung); validasi rute di antaranya dan fast travel berbiaya.
- [x] Implementasikan encounter pacing 90 detik atau 1,5–2 mil laut dengan satu aturan pemicu, cooldown, batas simultan dan opsi menghindar.
- [x] Implementasikan formula fast travel berdasarkan sektor (500 Rp per sektor) plus ransum harian per kru; tampilkan quote dan tes debit/kemajuan kalender/risiko rute.
- [x] Kunci kontrak dompet Rupiah `int64`; inventarisasi dan migrasikan API `PlayerGold`/`Gold` pada trade, buyout, repair, save, UI dan quest tanpa saldo otoritatif ganda.
- [ ] Tentukan satu aturan pembulatan untuk harga/biaya integer Rupiah dan tes saldo nol, nilai ekstrem, overflow/underflow, serta transaksi gagal atomik.
- [ ] Konsistenkan istilah sejarah alternatif di Codex/dialog: “Kongsi Dagang Barat”; jangan menyebut VOC historis sebagai faksi 1521 tanpa penjelasan timeline alternatif.
- [ ] Pertahankan plugin core engine yang dibutuhkan; tunda semua plugin pihak ketiga sampai vertical slice lulus kualifikasi.
- [ ] Migrasikan save lokal `LanunSaveSlot_0` tanpa kehilangan data ke schema-versioned Offline-First state; terapkan staging, validasi baca balik dan primary/backup atomik (`SaveSlot.sav`/`.bak`) pada `%LOCALAPPDATA%/Tides_Of_Nusantara/Saved/SaveGames/`.
- [x] Tambahkan `SaveSchemaVersion` dan migrator eksplisit; validasi migrasi `FOfficerSaveState` → `FOfficerLegacyState` sebelum menyimpan/mengganti file lama.
- [ ] Tambahkan uji recovery ketika primary save invalid, backup valid, keduanya invalid, atau proses penyimpanan terputus; jangan menghapus save lama sebelum pengganti valid.

### Tahap 1 — Vertical slice cerita (30–45 menit)

- [ ] Prolog desa/dermaga dengan traversal sederhana, stamina, satu aksi combat/defense, dan objektif yang dapat diselesaikan.
- [ ] Teluk awal, dialog/rekrut Daeng Rannu, resource pickup, workbench dan launch kapal.
- [ ] Perjalanan pendek dengan angin/cuaca dasar dan satu encounter laut.
- [ ] Broadside, damage yang terkonfirmasi pada target, boarding, reward dan transisi kembali yang dapat dimainkan.
- [ ] UI objective, inventory minimal, checkpoint dan save/load lokal.

### Tahap 2 — Loop ekonomi dan wilayah

- [ ] Dompet Rupiah terpusat, inventory, cargo, dan pasar di dua pelabuhan.
- [ ] Harga/stok yang berubah dan transaksi buy/sell yang tervalidasi serta tersimpan.
- [ ] Quest Belenggu Suratan Utang bercabang; simpan pilihan dan status tahapnya.
- [ ] Buka Beli Kampung hanya sesudah utang bebas; pembelian tetap membayar harga hak kelola.
- [x] Implementasikan tiga domain reputasi terpisah (-100…+100): `FactionReputation`, `TerritoryPopularity`, `CrewLoyalty`; uji pengaruh masing-masing pada encounter, sewa/buyout, efisiensi kru dan mutiny.
- [ ] Implementasikan `Unrest` sebagai state risiko pemerintahan yang terpisah, dengan tindakan, ambang dan konsekuensi yang dapat dijelaskan.
- [x] Terapkan maintenance drag 28% dari gross penjualan kargo rempah dengan rincian gross/potongan/net tanpa double-charge.
- [x] Jalankan pemasukan pos kampung/tambang mingguan melalui kalender dengan biaya operasi lebih dahulu, berhenti/berkurang jika kas tidak cukup.
- [x] Proses tagihan setiap Senin 00:00 waktu game: upah kru + 2.500 Rp per benteng + 1.200 Rp per tambang; validasi kekurangan kas tanpa saldo negatif dan dengan notifikasi/konsekuensi.
- [x] Terapkan formula sewa `BaseRent × (1 − TerritoryPopularity/200)`, syarat buyout TerritoryPopularity ≥40 plus quest utang Completed, dan uji nilai batas -100/+100.
- [x] Uji ambang faksi (Hostile <-30, Neutral -30…+30, Allied >+30) dan mutiny harian CrewLoyalty <-50 dengan peluang `clamp(abs(CrewLoyalty+50)×1.5%,0%,100%)`.
- [ ] Implementasikan satu fasilitas kota yang dapat di-upgrade dengan biaya dan efek terukur.

### Tahap 3 — Companion, konten darat, dan progresi

- [ ] Implementasikan roster empat Companion inti, satu quest loyalitas, penugasan, fatigue dan manfaat.
- [ ] Integrasikan reward quest yang aman: modifier ber-cap, aktif hanya pada state penugasan yang benar.
- [ ] Implementasikan level Hero/Companion/satwa/markas dengan kurva diuji, UI XP dan milestone.
- [x] Implementasikan utility XP Hero `floor(500 + 50L + 9L^1.8)` (L=1–99, cap 100) dengan tes threshold per level dan cumulative golden values pada tabel.
- [x] Implementasikan progresi Hero persisten: Level 1 = 0 poin; Level 2–100 masing-masing +3 AP/+1 SP, prasyarat unlock, authority lokal dan aturan respec.
- [x] Implementasikan biaya skill point Tier 1/2/3/4 = 1/2/3/5 SP; respec di Somba Opu seharga Level × 1.000 Rp, cooldown 1 minggu, refund seluruh alokasi dan tes batas/persistensi.
- [x] Implementasikan diminishing returns atribut raw 1–50 (1,0x), 51–80 (0,5x), 81–100 (0,25x), effective cap 70; uji syarat unlock tetap memakai raw attribute.
- [ ] Integrasikan empat cabang skill ke AbilitySpec, input, biaya stamina, GameplayEffects, feedback UI, dan aturan stacking; bedakan progresi Hero dari AttributeSet AI.
- [x] Implementasikan ability Tangkas Salawaku end-to-end: parry window, mitigasi serangan incoming, event parry sukses, counter satu kali, cancellation/timer cleanup, authority lokal dan test.
- [ ] Buat traversal darat, satu fauna/mount yang sesuai, dan satu treasure quest.
- [ ] Validasi kebijakan folklore fiksi dan Protected Wildlife: Harimau Sumatra, Anoa, Komodo, Kakatua Raja tidak dapat diburu untuk grinding.
- [ ] Implementasikan pengaturan lokal Camera Shake 0–100% (default 100%), Motion Blur toggle, Reduced Flash, dan HUD scale 80–120%; verifikasi semua efek/render/UI benar-benar mengikuti setting.

### Tahap 4 — Simulasi makro dan endgame

- [ ] Armada pengiring dan formasi; governor/garrison, siege dan faksi dinamis.
- [ ] Trade routes abstrak, modal kerja, biaya, risiko, distress signal dan auto-resolve.
- [ ] Visual proxy saat pemain dekat dan rekonsiliasi state saat kapal kembali off-screen.
- [ ] Implementasikan rival/Nemesis dengan memori encounter terbatas, transisi hubungan, pilihan setelah kalah, dan counter-tactics yang memiliki counterplay.
- [ ] Implementasikan `URivalNemesisSubsystem` beserta header/source, tes transisi relasi dan memori taktis, serta persistensi campaign/save; jangan menganggap WorldSubsystem sebagai penyimpanan lintas sesi.
- [ ] Implementasikan simulasi faksi/event berbasis interval kampanye, state persisten, batas event, dan dampak pasar yang memiliki cap/durasi.
- [ ] Validasi quest prosedural (maksimum 3 aktif, prasyarat, resolusi, konsekuensi, konteks dan anti-duplikasi).
- [ ] Implementasikan upgrade port, tambang/perkebunan, serta Legacy Score berbobot yang tidak bergantung pada saldo Rupiah.
- [ ] Implementasikan tiga jalur transisi Companion (mentor, penerus gelar, institusi) dengan penerusan semua ability/perk dan bonus yang sudah diperoleh.
- [ ] Versikan dan migrasikan save `FOfficerSaveState` ke `FOfficerLegacyState`; validasi ID successor serta cegah loyalty bonus bertumpuk lintas generasi.
- [ ] Implementasikan Legacy Lock Confirmation UI yang merinci nasib Companion/pet, penerus, institusi, aset dan efek; sediakan batal dan lindungi save lama.
- [ ] Implementasikan transisi generasi dengan proteksi save lama dan aturan pembulatan aset.
- [ ] Evaluasi API dialog Ollama yang sudah ada sebelum mengubah scope WorldSubsystem; tes JSON/error/timeout/NPC ID, fallback yang diberitahukan, dan opsi mematikan integrasi lokal.

### Tahap 5 — Produksi konten dan target lintas platform

- [ ] Questline lengkap, bioma regional, fauna, senjata/armor, tavern, folklore dan boss.
- [ ] Profil performa dan packaging PC; ukur target kualitas/perangkat.
- [ ] Android/iOS tetap FROZEN sampai Shipping PC stabil; pembukaan scope memerlukan keputusan baru dan profil performa perangkat.
- [ ] Online/cross-save tetap DEFERRED; tidak masuk deliverable Offline-First vertical slice.

### Definition of Done untuk mencentang TODO

Sebuah item baru boleh dicentang jika:

- [ ] Kode/aset sudah ada dan terhubung ke alur yang dimaksud.
- [ ] Build/compile yang sesuai lulus.
- [ ] Perilaku berhasil diuji di Unreal Editor/perangkat target atau test otomatis yang relevan.
- [ ] Save/load, kegagalan, batas nilai, dan interaksi dengan sistem terkait sudah diperiksa.
- [ ] UI/feedback menjelaskan hasil kepada pemain dan tidak menampilkan keberhasilan palsu.
- [ ] Dokumentasi/status di bagian audit diperbarui agar tidak mengklaim lebih dari implementasi.

## 11. Kriteria penerimaan vertikal pertama

Vertical slice dianggap dapat dimainkan bila:

- Proyek membuka peta startup yang benar tanpa error fatal.
- Pemain dapat berjalan, berinteraksi, naik kapal, mengendalikan layar/kemudi, dan turun dengan input yang konsisten.
- Kapal bergerak dan merespons angin/cuaca dengan cara yang dapat dipahami; kontak air/physics tidak membuat kapal tenggelam atau terlempar tak terkendali.
- Encounter dapat diselesaikan, target kapal menerima damage, boarding menghasilkan state yang benar, dan tidak ada cooldown yang macet.
- Quest menyimpan progres; transaksi mengubah saldo Rupiah serta stok/inventory dengan benar.
- Save/load memulihkan posisi aman, quest, dompet, kapal dan inventory; kegagalan save dilaporkan.
- Setiap save membuat backup yang valid; primary rusak dapat dipulihkan dari backup di lokasi per-user yang ditentukan.
- Tidak ada farming transaksi bebas risiko yang mengalahkan quest atau aktivitas utama.
- Build dan tes yang relevan lulus pada toolchain UE yang ditentukan.

## 12. Keputusan desain yang masih terbuka

- Pemetaan GUID lokal `EngineAssociation` ke UE 5.8.4 Source Build dan validasi plugin pada source build.
- Playtest skala sektor, performa RTX 2000 Ada, waktu perjalanan, fast travel, kepadatan encounter dan penggunaan VRAM.
- Keseimbangan biaya operasi mingguan, reward XP, cap produksi, maintenance drag 28% dan risiko rute perlu playtest; formula dan baseline awal sudah ditetapkan.
- Dampak diminishing returns, respec, ambang reputasi dan mutiny, serta unjuk rasa wilayah perlu playtest; formula baseline sudah ditetapkan.
- Detail rotasi/atomic replacement, checksum/integrity policy, schema migration serta recovery primary/backup perlu diuji pada Windows.
- Kebijakan representasi budaya/fauna dan rating konten tetap membutuhkan riset/konsultasi; opsi aksesibilitas sudah ditetapkan tetapi perlu verifikasi UI/perangkat.

---

**Aturan status README/TODO:** sebelum fitur hadir di kode/aset, terhubung ke loop permainan, diuji, dan dapat dimainkan, sebut sebagai **rencana** atau **prototipe**—bukan selesai. Konfigurasi target boleh dicatat sebagai keputusan, tetapi plugin pruning/build/packaging tetap belum selesai sampai divalidasi. Checklist hanya status kerja; bukan pengganti validasi build dan playtest.

---

## 13. Sistem Maritim & Pedalaman Terpadu (Dual-Realm Gameplay)

Pengembangan prinsip desain Lanun Jaladri memperluas eksplorasi dari sekadar laut lepas menjadi ekosistem daratan dan laut yang terintegrasi.

### 1. Dwilogi Maritim & Pedalaman (Seamless Land-Sea Ecosystem)

**Laut adalah Urat Nadi, Daratan adalah Jantung Dunia:** Laut lepas adalah ruang pertempuran armada, navigasi angin muson, dan rute kargo antarpulau. Daratan (hutan hujan, rawa bakau, lembah karst, gua pesisir, hingga benteng kedatuan pedalaman) adalah tempat tersembunyinya sumber daya langka, situs purba, artefak pusaka, dan ruang ekspansi teritorial.

**Transisi Tanpa Jeda (Seamless Transition):** Pemain dapat merapatkan perahu di teluk tersembunyi, melempar jangkar, dan langsung melompat melintasi air dangkal menuju daratan tanpa loading screen yang memutus imersi.

### 2. Navigasi Sungai & DAS (Riverine Traversal)

**Perahu Besar di Samudra, Perahu Kecil di Sungai:** Kapal berukuran besar (Phinisi, Jung) dikuasai untuk samudra lepas, sedangkan perahu ber-draft dangkal (Lancang, Pencalang, Sampan) digunakan untuk menyusuri Daerah Aliran Sungai (DAS) besar di Sumatra, Kalimantan, dan Papua.

**Menerobos Kedalaman Nusantara:** Jalur sungai menjadi satu-satunya akses untuk menjangkau pasar pedalaman, pos tambang emas aluvial, serta desa agraris yang terisolasi dari perairan laut lepas.

### 3. Eksplorasi & Traversal Darat Bertingkat (Verticality & Wilderness Survival)

**Variasi Medan & Bioma:** Traversal darat tidak hanya berjalan kaki, melanjut menyusup di semak hutan bakau (Stealth), memanjat tebing karst/reruntuhan benteng tua (Parkour/Climbing), menyeberangi rawa, hingga menelusuri gua surut air laut (Subterranean Caves).

**Penunggangan Satwa & Angkutan Darat (Mounted Traversal):** Memanfaatkan kuda pesisir untuk pergerakan cepat di sabana/pantai, atau kerbau untuk mengangkut material berat melintasi medan lumpur pedalaman.

### 4. Interaksi Ekosistem Pesisir vs Pedalaman (Bandar vs Kedatuan)

**Dinamika Dua Masyarakat:**
- **Bandar Pesisir:** Heterogen, berfokus pada perdagangan internasional, artileri perunggu, armada bajak laut, dan pasar gelap.
- **Masyarakat Pedalaman:** Agraris, kaya akan deposit kayu besi/besi Luwu/emas, memegang teguh tradisi leluhur, serta dilindungi benteng kayu pesanggrahan.

**Rantai Pasok Saling Tergantung:** Blokade di laut menghentikan pasokan garam dan barang impor ke pedalaman, sebaliknya pemberontakan di pedalaman menghentikan pasokan beras, kayu ulin, dan bahan ransum ke kapal-kapal di pelabuhan.

### 5. Kru & Companion Berperan Ganda (Sea & Land Specialization)

**Dual Roles:** Setiap perwira memiliki spesialisasi tempur/utilitas laut maupun darat.
- Daeng Rannu: Nakhoda ramming di laut $\rightarrow$ Duel Keris & Komandan Boarding di darat.
- Kono: Master Scout perairan rawa $\rightarrow$ Pemburu racun sumpitan & penunjuk jalan hutan Arafura.
- Nyai Saraswati: Logistik palka kapal $\rightarrow$ Ahli racikan Jamu & pengolah hasil pertanian darat.

**Manajemen Istirahat & Kamp:** Saat mendarat di pulau tak berpenghuni, kru dapat membangun Temporary Camp untuk memulihkan fatigue, memasak kuliner tradisional (Rendang/Coto), dan merawat satwa companion.

### 6. Pilihan Ekspansi Berkonsekuensi (Aliansi, Buyout, & Penaklukan)

**Tiga Jalur Penguasaan Wilayah:**
- **Aliansi:** Membantu kesultanan/kedatuan lokal lewat quest untuk mendapatkan akses fasilitas tanpa merusak tatanan kota.
- **Beli Kampung (Buyout):** Membebaskan utang warga adat terlebih dahulu, lalu membayar hak kelola secara sah dengan Rupiah untuk loyalitas 100%.
- **Penaklukan (Conquest):** Membombardir pertahanan pantai dengan meriam broadside lalu melakukan Assault darat—menghasilkan kontrol cepat namun memicu unrest tinggi dan kerusakan fasilitas.

### 7. Proteksi Fauna & Folklore Berbasis Riset Budaya

**Pelestarian Satwa Endemik:** Satwa langka (Harimau Sumatra, Komodo, Anoa, Kasuari) diposisikan sebagai bagian ekologi, Pet Companion, atau sasaran penyelamatan habitat—bukan target perburuan grinding material.

**Folklore Atmosferik:** Teka-teki situs batu pusaka, Wafak Luwu, dan rumor mistis pedalaman dikemas sebagai bagian cerita dan elemen visual fiksi yang kaya atmosfer tanpa menggeneralisasi tradisi keagamaan nyata.

### 8. Progresi Membuka Pilihan & Kapasitas Strategis

**Bukan Sekadar Peningkatan Angka:** Naik level dan upgrade markas (darat/laut) tidak hanya menaikkan damage, melainkan membuka rute sungai baru, slot jimat Wafak tambahan, jenis perahu baru, akses pasar gelap, hingga kemampuan diplomasi lintas faksi.
