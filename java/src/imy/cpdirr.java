package imy;

import java.io.IOException;
import java.nio.file.*;
import java.nio.file.attribute.BasicFileAttributes;
import java.util.EnumSet;
import java.util.Iterator;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author mcangel
 */
public class cpdirr
{
	/**
	 * Deletes all files and subdirectories under dir.
	 * @param dir The directory that will be cleared.
	 * @return dir Path if all deletions were successful.
	 * @throws IOException If a deletion fails, the method stops attempting to delete and throws exception.
	 */
	public static Path clearDir(Path dir) throws IOException
	{
		//Check to ensure that the source is valid
		if (!Files.exists(dir))
		{
			throw new IOException("clearDir: Can not find dir: " + dir);
		} // check to ensure the source is a directory
		else if (!Files.isDirectory(dir))
		{
			throw new IOException("clearDir: dir isn't a directory: " + dir);
		}
		
		FileVisitor<Path> FileDeletor = new SimpleFileVisitor<Path>()
			{
				@Override
				public FileVisitResult visitFile(Path file, BasicFileAttributes attrs)
						throws IOException
				{
					Files.delete(file);
					return FileVisitResult.CONTINUE;
				}

				@Override
				public FileVisitResult postVisitDirectory(Path dir, IOException e)
						throws IOException
				{
					if (e == null)
					{
						Files.delete(dir);
						return FileVisitResult.CONTINUE;
					} else
					{
						// directory iteration failed
						throw e;
					}
				}
			};
		try(DirectoryStream<Path> ds = Files.newDirectoryStream(dir))
		{
			Iterator<Path> dsi = ds.iterator();
			while(dsi.hasNext())
			{
				Files.walkFileTree(dsi.next(), 
						EnumSet.noneOf(FileVisitOption.class), 
						Integer.MAX_VALUE, FileDeletor);
			}
		}
		return dir;
	}
	
	/**
	 * Copies all source directory tree to the target path.
	 * @param source	- path to copy from
	 * @param target	- path to copy to
	 * @return target path
	 * @throws IOException 
	 */
	public static Path cpdirtree(final Path source, final Path target) throws IOException
	{
		//Check to ensure that the source is valid
		if (!Files.exists(source))
		{
			throw new IOException("cpdirtree: Can not find source: " + source);
		}
		// check to ensure the source is a directory
		else if (!Files.isDirectory(source))
		{
			throw new IOException("cpdirtree: Source isn't a directory: " + source);
		}
		//check to ensure we have rights to the source...
		else if (!Files.isReadable(source))
		{
			throw new IOException("cpdirtree: No right to source: " + source);
		}
		
		//does the destination already exist?
		if (!Files.exists(target))
		{
			//if not we need to make it exist if possible
			Files.createDirectories(target);
		}

		Files.walkFileTree(source, new SimpleFileVisitor<Path>()
			{
				@Override
				public FileVisitResult preVisitDirectory(Path dir, BasicFileAttributes attrs)
						throws IOException
				{
					Path targetdir = target.resolve(source.relativize(dir));
					try
					{
						Files.copy(dir, targetdir, StandardCopyOption.COPY_ATTRIBUTES);
					} catch (FileAlreadyExistsException e)
					{
						if (!Files.isDirectory(targetdir))
						{
							throw e;
						}
					}
					return FileVisitResult.CONTINUE;
				}
			});
		return target;
	}

	
	/**
	 * @param args the command line arguments
	 */
	public static void main(String[] args)
	{
		String usage = "Usage: [-d] SRC DST\n"
				+ "\t-d     to clear destination directory\n"
				+ "\tSRC    path to directory to copy from\n"
				+ "\tDST    path to directory to copy to\n";
		boolean clearDst = false;
		Path src;
		Path dst;
		// Ensure necessary arguments passed.
		if (args.length < 2)
		{
			System.err.println(usage);
			System.exit(0);
		}
		// Directory clearence checking
		if(args[0].equals("-d"))
		{
			// Ensure necessary arguments passed.
			if (args.length < 3)
			{
				System.err.println(usage);
				System.exit(0);
			}
			src = FileSystems.getDefault().getPath(args[1]);
			dst = FileSystems.getDefault().getPath(args[2]);
			clearDst = true;
		}
		else
		{
			src = FileSystems.getDefault().getPath(args[0]);
			dst = FileSystems.getDefault().getPath(args[1]);
		}
		// clear the directory (if asked) and copy source tree
		try
		{
			if (clearDst)
			{
				clearDir(dst);
			}
			cpdirtree(src, dst);
			System.out.println("Copying done.");
		} catch (IOException ex)
		{
			Logger.getLogger(cpdirr.class.getName()).log(Level.SEVERE, null, ex);
		}
	}
}
