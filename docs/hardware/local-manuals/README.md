# Local Hardware Manuals

This directory is for local-only vendor manuals that must not be committed to Git.

## Local files currently expected

| File | Size | Notes |
| --- | ---: | --- |
| `Hyper982_RTK_Module.pdf` | 1,718,055 bytes | Vendor RTK module manual. Binary scan found external sharing links, including an extraction-code style URL, so keep it local. |
| `HyperLTE_4G_Module.pdf` | 1,090,540 bytes | Vendor 4G module manual. Binary scan found external video/share links, so keep it local. |

## Handling rules

- Do not commit PDF manuals without explicit confirmation.
- Do not use Git LFS for these files without explicit confirmation.
- Do not copy these PDFs into release packages.
- Keep only non-sensitive metadata in Git.
- If a manual contains server, account, password, RTSP, or provisioning credentials, document that fact without copying the credential value into Git.
